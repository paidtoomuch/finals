#pragma once 
namespace utilities
{
	class c_utilities
	{
	public:

		std::uintptr_t game_base;
		std::uintptr_t find_obj;
		std::uintptr_t font;
		std::uintptr_t engine;

	public:
		bool is_bad_pointer( ULONG64 Pointer )
		{
			if ( ( Pointer < 0xFFFFFFFFFF ) || ( Pointer > 0x2FFFFFFFFFF ) )
				return true;

			else return false;
		}

		void attach_console( const char* title );
	};
} inline std::shared_ptr<utilities::c_utilities> m_util = std::make_shared<utilities::c_utilities>( );