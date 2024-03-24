#pragma once

extern "C" long nt_read_virtual_memory( HANDLE process, void* address, void* buffer, std::size_t number_of_bytes_to_read, std::size_t * number_of_bytes_readed );
extern "C" long nt_write_virtual_memory( HANDLE process, void* address, void* buffer, std::size_t number_of_bytes_to_write, std::size_t * number_of_bytes_written );
extern "C" long nt_protect_virtual_memory( HANDLE process, PVOID * address, PULONG number_of_bytes_to_protect, ULONG new_protection, PULONG old_protection ); //crash


namespace mem {

    static bool read_virtual_memory( std::uintptr_t address, std::uint8_t* buffer, std::size_t size ) {
        std::size_t number_of_bytes_readed = {};
        return nt_read_virtual_memory( HANDLE( -1 ), reinterpret_cast< void* >( address ), buffer, size, &number_of_bytes_readed ) == 0;
    }

    static bool write_virtual_memory( std::uintptr_t address, std::uint8_t* buffer, std::size_t size ) {
        std::size_t number_of_bytes_written = {};
        return nt_write_virtual_memory( HANDLE( -1 ), reinterpret_cast< void* >( address ), buffer, size, &number_of_bytes_written ) == 0;
    }

    template <typename t>
    t read( std::uintptr_t addr ) {
        t response{ };

        read_virtual_memory(
            addr,
            ( std::uint8_t* ) &response,
            sizeof( t )
        );

        return response;
    }

    template <typename type> static bool write( void* address, type value ) {
        return write_virtual_memory( ( std::uintptr_t ) address, reinterpret_cast< uint8_t* >( &value ), sizeof( type ) );
    }
}