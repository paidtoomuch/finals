/*
 * Copyright 2017 - 2021 Justas Masiulis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HASH_HPP
#define HASH_HPP

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#include <arm_neon.h>
#elif defined(_M_X64) || defined(__amd64__) || defined(_M_IX86) || defined(__i386__)
#include <immintrin.h>
#else
#error Unsupported platform
#endif

#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>

#define OBFUSCATE_STR(str) ::jm::xor_string([]() { return str; }, std::integral_constant<std::size_t, sizeof(str) / sizeof(*str)>{}, std::make_index_sequence<::jm::detail::_buffer_size<sizeof(str)>()>{}).crypt_get()

#ifdef _MSC_VER
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline)) inline
#endif

#if defined(__clang__) || defined(__GNUC__)
#define JM_XORSTR_LOAD_FROM_REG(x) ::jm::detail::load_from_reg(x)
#else
#define JM_XORSTR_LOAD_FROM_REG(x) (x)
#endif

namespace jm {

    namespace detail {

        template<std::size_t Size>
        XORSTR_FORCEINLINE constexpr std::size_t _buffer_size()
        {
            return ((Size / 16) + (Size % 16 != 0)) * 2;
        }

        template<std::uint32_t Seed>
        XORSTR_FORCEINLINE constexpr std::uint32_t key4() noexcept
        {
            std::uint32_t value = Seed;
            for (char c : __TIME__)
                value = static_cast<std::uint32_t>((value ^ c) * 16777619ull);
            return value;
        }

        template<std::size_t S>
        XORSTR_FORCEINLINE constexpr std::uint64_t key8()
        {
            constexpr auto first_part = key4<2166136261 + S>();
            constexpr auto second_part = key4<first_part>();
            return (static_cast<std::uint64_t>(first_part) << 32) | second_part;
        }

        // loads up to 8 characters of string into uint64 and xors it with the key
        template<std::size_t N, class CharT>
        XORSTR_FORCEINLINE constexpr std::uint64_t
            load_xored_str8(std::uint64_t key, std::size_t idx, const CharT* str) noexcept
        {
            using cast_type = typename std::make_unsigned<CharT>::type;
            constexpr auto value_size = sizeof(CharT);
            constexpr auto idx_offset = 8 / value_size;

            std::uint64_t value = key;
            for (std::size_t i = 0; i < idx_offset && i + idx * idx_offset < N; ++i)
                value ^=
                (std::uint64_t{ static_cast<cast_type>(str[i + idx * idx_offset]) }
            << ((i % idx_offset) * 8 * value_size));

            return value;
        }

        // forces compiler to use registers instead of stuffing constants in rdata
        XORSTR_FORCEINLINE std::uint64_t load_from_reg(std::uint64_t value) noexcept
        {
#if defined(__clang__) || defined(__GNUC__)
            asm("" : "=r"(value) : "0"(value) : );
#endif
            return value;
        }

        template<std::uint64_t V>
        struct uint64_v {
            constexpr static std::uint64_t value = V;
        };

    } // namespace detail

    template<class CharT, std::size_t Size, class Keys, class Indices>
    class xor_string;

    template<class CharT, std::size_t Size, std::uint64_t... Keys, std::size_t... Indices>
    class xor_string<CharT, Size, std::integer_sequence<std::uint64_t, Keys...>, std::index_sequence<Indices...>> {
#ifndef JM_XORSTR_DISABLE_AVX_INTRINSICS
        constexpr static inline std::uint64_t alignment = ((Size > 16) ? 32 : 16);
#else
        constexpr static inline std::uint64_t alignment = 16;
#endif

        alignas(alignment) std::uint64_t _storage[sizeof...(Keys)];

    public:
        using value_type = CharT;
        using size_type = std::size_t;
        using pointer = CharT*;
        using const_pointer = const CharT*;

        template<class L>
        XORSTR_FORCEINLINE xor_string(L l, std::integral_constant<std::size_t, Size>, std::index_sequence<Indices...>) noexcept
            : _storage{ JM_XORSTR_LOAD_FROM_REG(detail::uint64_v<detail::load_xored_str8<Size>(Keys, Indices, l())>::value)... }
        {}

        XORSTR_FORCEINLINE constexpr size_type size() const noexcept
        {
            return Size - 1;
        }

        XORSTR_FORCEINLINE void crypt() noexcept
        {
            // everything is inlined by hand because a certain compiler with a certain linker is _very_ slow
#if defined(__clang__)
            alignas(alignment)
                std::uint64_t arr[]{ JM_XORSTR_LOAD_FROM_REG(Keys)... };
            std::uint64_t* keys =
                (std::uint64_t*)JM_XORSTR_LOAD_FROM_REG((std::uint64_t)arr);
#else
            alignas(alignment) std::uint64_t keys[]{ JM_XORSTR_LOAD_FROM_REG(Keys)... };
#endif

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#if defined(__clang__)
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : __builtin_neon_vst1q_v(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(__builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2, 51),
                    __builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(keys) + Indices * 2, 51)),
                51)), ...);
#else // GCC, MSVC
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : vst1q_u64(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(vld1q_u64(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2),
                    vld1q_u64(reinterpret_cast<const uint64_t*>(keys) + Indices * 2)))), ...);
#endif
#elif !defined(JM_XORSTR_DISABLE_AVX_INTRINSICS)
            ((Indices >= sizeof(_storage) / 32 ? static_cast<void>(0) : _mm256_store_si256(
                reinterpret_cast<__m256i*>(_storage) + Indices,
                _mm256_xor_si256(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(_storage) + Indices),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(keys) + Indices)))), ...);

            if constexpr (sizeof(_storage) % 32 != 0)
                _mm_store_si128(
                    reinterpret_cast<__m128i*>(_storage + sizeof...(Keys) - 2),
                    _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage + sizeof...(Keys) - 2)),
                        _mm_load_si128(reinterpret_cast<const __m128i*>(keys + sizeof...(Keys) - 2))));
#else
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : _mm_store_si128(
                reinterpret_cast<__m128i*>(_storage) + Indices,
                _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage) + Indices),
                    _mm_load_si128(reinterpret_cast<const __m128i*>(keys) + Indices)))), ...);
#endif
        }

        XORSTR_FORCEINLINE const_pointer get() const noexcept
        {
            return reinterpret_cast<const_pointer>(_storage);
        }

        XORSTR_FORCEINLINE pointer get() noexcept
        {
            return reinterpret_cast<pointer>(_storage);
        }

        XORSTR_FORCEINLINE pointer crypt_get() noexcept
        {
            // crypt() is inlined by hand because a certain compiler with a certain linker is _very_ slow
#if defined(__clang__)
            alignas(alignment)
                std::uint64_t arr[]{ JM_XORSTR_LOAD_FROM_REG(Keys)... };
            std::uint64_t* keys =
                (std::uint64_t*)JM_XORSTR_LOAD_FROM_REG((std::uint64_t)arr);
#else
            alignas(alignment) std::uint64_t keys[]{ JM_XORSTR_LOAD_FROM_REG(Keys)... };
#endif

#if defined(_M_ARM64) || defined(__aarch64__) || defined(_M_ARM) || defined(__arm__)
#if defined(__clang__)
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : __builtin_neon_vst1q_v(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(__builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2, 51),
                    __builtin_neon_vld1q_v(reinterpret_cast<const uint64_t*>(keys) + Indices * 2, 51)),
                51)), ...);
#else // GCC, MSVC
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : vst1q_u64(
                reinterpret_cast<uint64_t*>(_storage) + Indices * 2,
                veorq_u64(vld1q_u64(reinterpret_cast<const uint64_t*>(_storage) + Indices * 2),
                    vld1q_u64(reinterpret_cast<const uint64_t*>(keys) + Indices * 2)))), ...);
#endif
#elif !defined(JM_XORSTR_DISABLE_AVX_INTRINSICS)
            ((Indices >= sizeof(_storage) / 32 ? static_cast<void>(0) : _mm256_store_si256(
                reinterpret_cast<__m256i*>(_storage) + Indices,
                _mm256_xor_si256(
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(_storage) + Indices),
                    _mm256_load_si256(reinterpret_cast<const __m256i*>(keys) + Indices)))), ...);

            if constexpr (sizeof(_storage) % 32 != 0)
                _mm_store_si128(
                    reinterpret_cast<__m128i*>(_storage + sizeof...(Keys) - 2),
                    _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage + sizeof...(Keys) - 2)),
                        _mm_load_si128(reinterpret_cast<const __m128i*>(keys + sizeof...(Keys) - 2))));
#else
            ((Indices >= sizeof(_storage) / 16 ? static_cast<void>(0) : _mm_store_si128(
                reinterpret_cast<__m128i*>(_storage) + Indices,
                _mm_xor_si128(_mm_load_si128(reinterpret_cast<const __m128i*>(_storage) + Indices),
                    _mm_load_si128(reinterpret_cast<const __m128i*>(keys) + Indices)))), ...);
#endif

            return (pointer)(_storage);
        }
    };

    template<class L, std::size_t Size, std::size_t... Indices>
    xor_string(L l, std::integral_constant<std::size_t, Size>, std::index_sequence<Indices...>) -> xor_string<
        std::remove_const_t<std::remove_reference_t<decltype(l()[0])>>,
        Size,
        std::integer_sequence<std::uint64_t, detail::key8<Indices>()...>,
        std::index_sequence<Indices...>>;

} // namespace jm

typedef unsigned long long QWORD;

#pragma warning (disable: 4554)
#pragma warning (disable: 4244)
#pragma warning (disable: 4505)

constexpr QWORD _s1 = __TIME__[6] - 48;
constexpr QWORD _s2 = __TIME__[7] - 48;
constexpr QWORD _h1 = __TIME__[0] - 48;
constexpr QWORD _h2 = __TIME__[1] - 48;
constexpr QWORD _m1 = __TIME__[3] - 48;
constexpr QWORD _m2 = __TIME__[4] - 48;

constexpr QWORD __tvtodw(QWORD high, QWORD low) { return high * 10 + low; }
constexpr QWORD _HOURS = __tvtodw(_h1, _h2) + 1;
constexpr QWORD _MINUTES = __tvtodw(_m1, _m2);
constexpr QWORD _SECONDS = __tvtodw(_s1, _s2);
constexpr QWORD _TIMEVAL = (_SECONDS + (_MINUTES * 60) + (_HOURS * 3600));

__declspec(noinline) static QWORD __unhideDWORD(volatile QWORD val) {
    volatile QWORD x = val ^ _SECONDS;
    volatile QWORD part1 = _SECONDS;
    volatile QWORD part2 = (_MINUTES ^ _m2) * _s2;
    volatile QWORD part3 = (_s1 * _MINUTES / _HOURS);
    volatile QWORD partEnd1 = (part1 + part2 + 1);
    volatile QWORD partEnd2 = x - part3;
    volatile QWORD partEnd = partEnd2 / partEnd1;
    return partEnd;
}

#define __hiddenDWORD(x) (((x * _SECONDS) + (x * (_MINUTES ^ _m2) * _s2) + (_s1 * _MINUTES / _HOURS) + x) ^ _SECONDS)
#define OBFUSCATE_NUM(x) (__unhideDWORD(__hiddenDWORD(x)))

#endif // include guard



#if defined(__GNUC__)
typedef          long long ll;
typedef unsigned long long ull;
#define __int64 long long
#define __int32 int
#define __int16 short
#define __int8  char
#define MAKELL(num) num ## LL
#define FMT_64 "ll"
#elif defined(_MSC_VER)
typedef          __int64 ll;
typedef unsigned __int64 ull;
#define MAKELL(num) num ## i64
#define FMT_64 "I64"
#elif defined (__BORLANDC__)
typedef          __int64 ll;
typedef unsigned __int64 ull;
#define MAKELL(num) num ## i64
#define FMT_64 "L"
#else
#error "unknown compiler"
#endif

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

// Partially defined types. They are used when the decompiler does not know
// anything about the type except its size.
#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64
#if !defined(_MSC_VER)
#define _LONGLONG __int128
#endif

// Non-standard boolean types. They are used when the decompiler can not use
// the standard "bool" type because of the size mistmatch but the possible
// values are only 0 and 1. See also 'BOOL' type below.
typedef int8 _BOOL1;
typedef int16 _BOOL2;
typedef int32 _BOOL4;

//#ifndef _WINDOWS_
//typedef int8 BYTE;
//typedef int16 WORD;
//typedef int32 DWORD;
//typedef int32 LONG;
//typedef int BOOL;       // uppercase BOOL is usually 4 bytes
//#endif
//typedef int64 QWORD;
#ifndef __cplusplus
typedef int bool;       // we want to use bool in our C programs
#endif

#define __pure          // pure function: always returns the same value, has no
// side effects

// Non-returning function
#if defined(__GNUC__)
#define __noreturn  __attribute__((noreturn))
#else
#define __noreturn  __declspec(noreturn)
#endif


#ifndef NULL
#define NULL 0
#endif

// Some convenience macros to make partial accesses nicer
#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
#  define LOW_IND(x,part_type)   LAST_IND(x,part_type)
#  define HIGH_IND(x,part_type)  0
#else
#  define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
#  define LOW_IND(x,part_type)   0
#endif
// first unsigned macros:
//#define LODWORD(x)  (*((DWORD*)&(x)))  // low dword
//#define HIDWORD(x)  (*((DWORD*)&(x)+1))

#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define DWORDn(x, n)  (*((_DWORD*)&(x)+n))

#define LOBYTE(x)  BYTEn(x,LOW_IND(x,_BYTE))
#define LOWORD(x)  WORDn(x,LOW_IND(x,_WORD))
#define LODWORD(x) DWORDn(x,LOW_IND(x,_DWORD))
#define HIBYTE(x)  BYTEn(x,HIGH_IND(x,_BYTE))
#define HIWORD(x)  WORDn(x,HIGH_IND(x,_WORD))
#define HIDWORD(x) DWORDn(x,HIGH_IND(x,_DWORD))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTE4(x)   BYTEn(x,  4)
#define BYTE5(x)   BYTEn(x,  5)
#define BYTE6(x)   BYTEn(x,  6)
#define BYTE7(x)   BYTEn(x,  7)
#define BYTE8(x)   BYTEn(x,  8)
#define BYTE9(x)   BYTEn(x,  9)
#define BYTE10(x)  BYTEn(x, 10)
#define BYTE11(x)  BYTEn(x, 11)
#define BYTE12(x)  BYTEn(x, 12)
#define BYTE13(x)  BYTEn(x, 13)
#define BYTE14(x)  BYTEn(x, 14)
#define BYTE15(x)  BYTEn(x, 15)
#define WORD1(x)   WORDn(x,  1)
#define WORD2(x)   WORDn(x,  2)         // third word of the object, unsigned
#define WORD3(x)   WORDn(x,  3)
#define WORD4(x)   WORDn(x,  4)
#define WORD5(x)   WORDn(x,  5)
#define WORD6(x)   WORDn(x,  6)
#define WORD7(x)   WORDn(x,  7)

// now signed macros (the same but with sign extension)
#define SBYTEn(x, n)   (*((int8*)&(x)+n))
#define SWORDn(x, n)   (*((int16*)&(x)+n))
#define SDWORDn(x, n)  (*((int32*)&(x)+n))

#define SLOBYTE(x)  SBYTEn(x,LOW_IND(x,int8))
#define SLOWORD(x)  SWORDn(x,LOW_IND(x,int16))
#define SLODWORD(x) SDWORDn(x,LOW_IND(x,int32))
#define SHIBYTE(x)  SBYTEn(x,HIGH_IND(x,int8))
#define SHIWORD(x)  SWORDn(x,HIGH_IND(x,int16))
#define SHIDWORD(x) SDWORDn(x,HIGH_IND(x,int32))
#define SBYTE1(x)   SBYTEn(x,  1)
#define SBYTE2(x)   SBYTEn(x,  2)
#define SBYTE3(x)   SBYTEn(x,  3)
#define SBYTE4(x)   SBYTEn(x,  4)
#define SBYTE5(x)   SBYTEn(x,  5)
#define SBYTE6(x)   SBYTEn(x,  6)
#define SBYTE7(x)   SBYTEn(x,  7)
#define SBYTE8(x)   SBYTEn(x,  8)
#define SBYTE9(x)   SBYTEn(x,  9)
#define SBYTE10(x)  SBYTEn(x, 10)
#define SBYTE11(x)  SBYTEn(x, 11)
#define SBYTE12(x)  SBYTEn(x, 12)
#define SBYTE13(x)  SBYTEn(x, 13)
#define SBYTE14(x)  SBYTEn(x, 14)
#define SBYTE15(x)  SBYTEn(x, 15)
#define SWORD1(x)   SWORDn(x,  1)
#define SWORD2(x)   SWORDn(x,  2)
#define SWORD3(x)   SWORDn(x,  3)
#define SWORD4(x)   SWORDn(x,  4)
#define SWORD5(x)   SWORDn(x,  5)
#define SWORD6(x)   SWORDn(x,  6)
#define SWORD7(x)   SWORDn(x,  7)


// Helper functions to represent some assembly instructions.

#ifdef __cplusplus

// compile time assertion
#define __CASSERT_N0__(l) COMPILE_TIME_ASSERT_ ## l
#define __CASSERT_N1__(l) __CASSERT_N0__(l)
#define CASSERT(cnd) typedef char __CASSERT_N1__(__LINE__) [(cnd) ? 1 : -1]

// check that unsigned multiplication does not overflow
template<class T> bool is_mul_ok( T count, T elsize )
{
    CASSERT( ( T ) ( -1 ) > 0 ); // make sure T is unsigned
    if ( elsize == 0 || count == 0 )
        return true;
    return count <= ( ( T ) ( -1 ) ) / elsize;
}

// multiplication that saturates (yields the biggest value) instead of overflowing
// such a construct is useful in "operator new[]"
template<class T> bool saturated_mul( T count, T elsize )
{
    return is_mul_ok( count, elsize ) ? count * elsize : T( -1 );
}

#include <stddef.h> // for size_t
#include <cstdint>

// memcpy() with determined behavoir: it always copies
// from the start to the end of the buffer
// note: it co
// s byte by byte, so it is not equivalent to, for example, rep movsd
inline void *qmemcpy( void *dst, const void *src, size_t cnt )
{
    char *out = ( char * ) dst;
    const char *in = ( const char * ) src;
    while ( cnt > 0 )
    {
        *out++ = *in++;
        --cnt;
    }
    return dst;
}

// Generate a reference to pair of operands
template<class T>  int16 __PAIR__( int8  high, T low ) { return ( ( ( int16 ) high ) << sizeof( high ) * 8 ) | uint8( low ); }
template<class T>  int32 __PAIR__( int16 high, T low ) { return ( ( ( int32 ) high ) << sizeof( high ) * 8 ) | uint16( low ); }
template<class T>  int64 __PAIR__( int32 high, T low ) { return ( ( ( int64 ) high ) << sizeof( high ) * 8 ) | uint32( low ); }
template<class T> uint16 __PAIR__( uint8  high, T low ) { return ( ( ( uint16 ) high ) << sizeof( high ) * 8 ) | uint8( low ); }
template<class T> uint32 __PAIR__( uint16 high, T low ) { return ( ( ( uint32 ) high ) << sizeof( high ) * 8 ) | uint16( low ); }
template<class T> uint64 __PAIR__( uint32 high, T low ) { return ( ( ( uint64 ) high ) << sizeof( high ) * 8 ) | uint32( low ); }

// rotate left
template<class T> T __ROL__( T value, int count )
{
    const uint nbits = sizeof( T ) * 8;

    if ( count > 0 )
    {
        count %= nbits;
        T high = value >> ( nbits - count );
        if ( T( -1 ) < 0 )
            high &= ~( ( T( -1 ) << count ) );
        value <<= count;
        value |= high;
    }
    else
    {
        count = -count % nbits;
        T low = value << ( nbits - count );
        value >>= count;
        value |= low;
    }
    return value;
}

inline uint8  __ROL1__( uint8  value, int count ) { return __ROL__( ( uint8 ) value, count ); }
inline uint16 __ROL2__( uint16 value, int count ) { return __ROL__( ( uint16 ) value, count ); }
inline uint32 __ROL4__( uint32 value, int count ) { return __ROL__( ( uint32 ) value, count ); }
inline uint64 __ROL8__( uint64 value, int count ) { return __ROL__( ( uint64 ) value, count ); }
inline uint8  __ROR1__( uint8  value, int count ) { return __ROL__( ( uint8 ) value, -count ); }
inline uint16 __ROR2__( uint16 value, int count ) { return __ROL__( ( uint16 ) value, -count ); }
inline uint32 __ROR4__( uint32 value, int count ) { return __ROL__( ( uint32 ) value, -count ); }
inline uint64 __ROR8__( uint64 value, int count ) { return __ROL__( ( uint64 ) value, -count ); }

// carry flag of left shift
template<class T> int8 __MKCSHL__( T value, uint count )
{
    const uint nbits = sizeof( T ) * 8;
    count %= nbits;

    return ( value >> ( nbits - count ) ) & 1;
}

// carry flag of right shift
template<class T> int8 __MKCSHR__( T value, uint count )
{
    return ( value >> ( count - 1 ) ) & 1;
}

// sign flag
template<class T> int8 __SETS__( T x )
{
    if ( sizeof( T ) == 1 )
        return int8( x ) < 0;
    if ( sizeof( T ) == 2 )
        return int16( x ) < 0;
    if ( sizeof( T ) == 4 )
        return int32( x ) < 0;
    return int64( x ) < 0;
}

// overflow flag of subtraction (x-y)
template<class T, class U> int8 __OFSUB__( T x, U y )
{
    if ( sizeof( T ) < sizeof( U ) )
    {
        U x2 = x;
        int8 sx = __SETS__( x2 );
        return ( sx ^ __SETS__( y ) ) & ( sx ^ __SETS__( x2 - y ) );
    }
    else
    {
        T y2 = y;
        int8 sx = __SETS__( x );
        return ( sx ^ __SETS__( y2 ) ) & ( sx ^ __SETS__( x - y2 ) );
    }
}

// overflow flag of addition (x+y)
template<class T, class U> int8 __OFADD__( T x, U y )
{
    if ( sizeof( T ) < sizeof( U ) )
    {
        U x2 = x;
        int8 sx = __SETS__( x2 );
        return ( ( 1 ^ sx ) ^ __SETS__( y ) ) & ( sx ^ __SETS__( x2 + y ) );
    }
    else
    {
        T y2 = y;
        int8 sx = __SETS__( x );
        return ( ( 1 ^ sx ) ^ __SETS__( y2 ) ) & ( sx ^ __SETS__( x + y2 ) );
    }
}

// carry flag of subtraction (x-y)
template<class T, class U> int8 __CFSUB__( T x, U y )
{
    int size = sizeof( T ) > sizeof( U ) ? sizeof( T ) : sizeof( U );
    if ( size == 1 )
        return uint8( x ) < uint8( y );
    if ( size == 2 )
        return uint16( x ) < uint16( y );
    if ( size == 4 )
        return uint32( x ) < uint32( y );
    return uint64( x ) < uint64( y );
}

// carry flag of addition (x+y)
template<class T, class U> int8 __CFADD__( T x, U y )
{
    int size = sizeof( T ) > sizeof( U ) ? sizeof( T ) : sizeof( U );
    if ( size == 1 )
        return uint8( x ) > uint8( x + y );
    if ( size == 2 )
        return uint16( x ) > uint16( x + y );
    if ( size == 4 )
        return uint32( x ) > uint32( x + y );
    return uint64( x ) > uint64( x + y );
}



#else
// The following definition is not quite correct because it always returns
// uint64. The above C++ functions are good, though.
#define __PAIR__(high, low) (((uint64)(high)<<sizeof(high)*8) | low)
// For C, we just provide macros, they are not quite correct.
#define __ROL__(x, y) __rotl__(x, y)      // Rotate left
#define __ROR__(x, y) __rotr__(x, y)      // Rotate right
#define __CFSHL__(x, y) invalid_operation // Generate carry flag for (x<<y)
#define __CFSHR__(x, y) invalid_operation // Generate carry flag for (x>>y)
#define __CFADD__(x, y) invalid_operation // Generate carry flag for (x+y)
#define __CFSUB__(x, y) invalid_operation // Generate carry flag for (x-y)
#define __OFADD__(x, y) invalid_operation // Generate overflow flag for (x+y)
#define __OFSUB__(x, y) invalid_operation // Generate overflow flag for (x-y)
#endif

// No definition for rcl/rcr because the carry flag is unknown
#define __RCL__(x, y)    invalid_operation // Rotate left thru carry
#define __RCR__(x, y)    invalid_operation // Rotate right thru carry
#define __MKCRCL__(x, y) invalid_operation // Generate carry flag for a RCL
#define __MKCRCR__(x, y) invalid_operation // Generate carry flag for a RCR
#define __SETP__(x, y)   invalid_operation // Generate parity flag for (x-y)

// In the decom
// lation listing there are some objects declarared as _UNKNOWN
// because we could not determine their types. Since the C compiler does not
// accept void item declarations, we replace them by anything of our choice,
// for example a char:

#define _UNKNOWN char

//#ifdef _MSC_VER
//#define snprintf _snprintf
//#define vsnprintf _vsnprintf
