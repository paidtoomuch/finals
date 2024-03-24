#pragma once 
#include "..\..\includes.hpp"

auto utilities::c_utilities::attach_console( const char* title ) -> void
{
	AllocConsole( );
	freopen_s( reinterpret_cast< _iobuf** >( __acrt_iob_func( 0 ) ), ( "conin$" ), ( "r" ), __acrt_iob_func( 0 ) );
	freopen_s( reinterpret_cast< _iobuf** >( __acrt_iob_func( 1 ) ), ( "conout$" ), ( "w" ), __acrt_iob_func( 1 ) );
	freopen_s( reinterpret_cast< _iobuf** >( __acrt_iob_func( 2 ) ), ( "conout$" ), ( "w" ), __acrt_iob_func( 2 ) );

	SetConsoleTitleA( title );
}


