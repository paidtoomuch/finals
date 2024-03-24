#pragma once
#include "core/includes.hpp"


auto DllMain( void*, std::uint32_t call_reason, void* ) -> bool {
	
	if ( call_reason != 1 )
		return FALSE;

	//m_util.get( )->attach_console( 
	//	OBFUSCATE_STR( "embark-shell" ) 
	//);

	LOG( "shell started\n" );

	//auto time_now = std::chrono::high_resolution_clock::now( );

	if ( !m_stub.get( )->setup( ) ) {
		LOG( "failed to find setup the stub module.\n" );
		return FALSE;
	}

	LOG( "stub is placed\n" );

	if ( !m_vars.get( )->setup( ) ) {
		LOG( "failed to find all the required offsets.\n" );
		return FALSE;
	}

	LOG( "offsets are updated and correct.\n" );

	auto m_viewport_client = mem::read<uintptr_t>(
		( uintptr_t ) m_util.get( )->engine + m_nom.get( )->data.viewport
	);

	if ( !m_viewport_client ) {
		LOG( "failed to read viewport client.\n" );
		return FALSE;
	}

	render::render_frame_vmt.clone(
		( void* ) ( m_viewport_client + 0x50 )
	);
	
	render::render_frame_vmt.apply(
		render::render_frame_h,
		m_nom.get( )->data.render_index
	);

	//auto time_span = std::chrono::duration_cast< std::chrono::duration< float > >(
	//	std::chrono::high_resolution_clock::now( ) - time_now
	//);

	//LOG( "executor finished successfully in %fms, program will return to normal control-flow.\n", time_span.count( ) );

	return TRUE;
}