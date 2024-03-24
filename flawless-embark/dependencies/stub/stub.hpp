#ifndef STUB_HPP
#define STUB_HPP

#define GADGET_ADDRESS 0x655631F

extern "C" void* _spoofer_stub( );

namespace detail {

    template <typename Ret, typename... Args>
    static inline auto shellcode_stub_helper(
        const void* shell,
        Args... args
    ) -> Ret {
        auto fn = ( Ret( * )( Args... ) )( shell );
        return fn( args... );
    }

    template <std::size_t Argc, typename>
    struct argument_remapper {
        // At least 5 params
        template<
            typename Ret,
            typename First,
            typename Second,
            typename Third,
            typename Fourth,
            typename... Pack
        >
        static auto do_call(
            const void* shell,
            void* shell_param,
            First first,
            Second second,
            Third third,
            Fourth fourth,
            Pack... pack
        ) -> Ret {
            return shellcode_stub_helper<
                Ret,
                First,
                Second,
                Third,
                Fourth,
                void*,
                void*,
                Pack...
            >(
                shell,
                first,
                second,
                third,
                fourth,
                shell_param,
                nullptr,
                pack...
            );
        }
    };

    template <std::size_t Argc>
    struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>> {
        // 4 or less params
        template<
            typename Ret,
            typename First = void*,
            typename Second = void*,
            typename Third = void*,
            typename Fourth = void*
        >
        static auto do_call(
            const void* shell,
            void* shell_param,
            First first = First{},
            Second second = Second{},
            Third third = Third{},
            Fourth fourth = Fourth{}
        ) -> Ret {
            return shellcode_stub_helper<
                Ret,
                First,
                Second,
                Third,
                Fourth,
                void*,
                void*
            >(
                shell,
                first,
                second,
                third,
                fourth,
                shell_param,
                nullptr
            );
        }
    };
}


namespace stub
{
    template <typename ret, typename... args>
    static __forceinline auto callstack( void* gadget, ret( *function )( args... ), args... arguments ) -> ret
    {
        const auto m_gadget = gadget;

        if ( m_gadget )
        {
            struct stub_params {
                const void* gadget;
                void* function;
                void* reg;
            };

            stub_params ret_call = {
                m_gadget,
                reinterpret_cast< void* >( function )
            };

            using mapper = detail::argument_remapper<sizeof...( args ), void>;

            return mapper::template do_call<ret, args...>( ( const void* ) &_spoofer_stub, &ret_call, arguments... );
        }
    }

    /*
    
        warning: do not call any uobject operatives or function inside of the stub header, it will crash.

    */

	class c_stub_header
	{
    public:

        void* gadget_rbx;

	public:

        [[nodiscard]] const bool setup( )
        {
            const auto base = customs::get_module(
                L"Discovery.exe"
            );

            m_util.get( )->game_base = std::move( base );

            if ( base ) {

                this->gadget_rbx = ( void* ) ( base + 0x68D531F );
                return 1;
                //auto sig_gadget = ( void* ) pattern::find( "FF 65 59", base );

                //if ( sig_gadget )
                //{
                //    this->gadget_rbx = std::move( sig_gadget );

                //    return 1;
                //}
            }

            return 0;
        }

	};
} inline std::shared_ptr<stub::c_stub_header> m_stub = std::make_shared<stub::c_stub_header>( );

#endif 