#pragma once 

#include "..\includes.hpp"
#include <stack>

// constructor 
render::c_render::c_render(  )
{	
	
}

auto render::c_render::setup( ) -> bool {
	const auto m_canvas = reinterpret_cast< u_canvas* >( u_object::find_object(
		L"CanvasObject",
		reinterpret_cast< u_object* >( -1 )
	) );

	if ( !m_canvas ) {
		return 0;
	}

	LOG("found canvas object: %llx\n", m_canvas );

	m_framework.get( )->setup( m_canvas );
		
	return 1;
}

auto render::c_render::release_objects( ) -> void
{

}

auto render::c_render::clean_context( ) -> void
{

}

auto render::c_render::tick( ) -> void
{
	do
	{
		if ( !m_framework.get( )->current_canvas ) {
			setup( );
		}

		const auto viewport_client = mem::read<finals::u_game_viewport_client*>(
			( uintptr_t ) m_util.get( )->engine + m_nom.get( )->data.viewport
		);

		const auto world = viewport_client->world( );
		if ( !world ) {
			continue;
		}

		const auto game_state = world->game_state( );
		if ( !game_state ) {
			continue;
		}

		//const auto game_instance = world->game_instance( );
		//if ( !game_instance ) {
		//	continue;
		//}

		//MessageBoxA(0, "1", 0, 0 );

		//const auto local_player = game_instance->local_player_array( )[0];
		//if ( !local_player ) {
		//	MessageBoxA( 0, "-1", 0, 0 );

		//	continue;
		//}
		//MessageBoxA( 0, "2", 0, 0 );

		//const auto player_controller = local_player->controller( );
		//if ( !player_controller ) {
		//	MessageBoxA( 0, "-2", 0, 0 );
		//	continue;
		//}
		//MessageBoxA( 0, "3", 0, 0 );

		auto player_array = game_state->player_array( );

		for ( int i = 0; i < player_array.size( ); i++ ) {
			auto player_state = player_array[i];
			if ( !player_state ) {
				continue;
			}

			auto player_pawn = player_state->pawn_private( );
			if ( !player_pawn ) {
				continue;
			}

			//auto head_socket_location = player_pawn->get_location( );

			auto player_controller = player_pawn->controller( );
			//if ( ! player_controller ) {
			//	continue;
			//}

			//if ( player_controller->bIsLocalPlayerController( ) ) {
			//	m_engine.get( )->local_controller = player_controller;
			//}

	/*		if ( m_util.get( )->is_bad_pointer( ( ULONG64 ) m_engine.get( )->local_controller ) ) {
				continue;
			}*/

			//auto player_mesh = player_pawn->mesh( );
			//if ( m_util.get( )->is_bad_pointer( ( ULONG64 ) player_mesh ) ) {
			//	continue;
			//}

			uemath::fvector location;
			uemath::fvector BoxExtent;

			player_pawn->get_actor_bounds( true, location, BoxExtent, false );

			location = player_pawn->get_location( );

			float b = 2.0f;

			uemath::fvector location_foot = { location.x,location.y,location.z + ( ( BoxExtent.z ) / b ) };
			uemath::fvector location_head = { location.x,location.y,location.z - ( ( BoxExtent.z ) / b ) };

			uemath::fvector2d head_pos, foot_pos;

			uemath::flinearcolor Color = {};

			if ( player_controller->line_of_sight_to( player_pawn ) )
			{
				Color = { 0.9f,0.f,0.f,1.f };
			}
			else Color = { 0.f, 1.f, 0.f,1.f };


			if ( player_controller->project_world_location_to_screen( location_foot, &foot_pos ) &&
				player_controller->project_world_location_to_screen( location_head, &head_pos ) )
			{
				m_framework.get( )->text( L"Enemy", head_pos, Color, true, true );
				//const float h = abs( head_pos.y - foot_pos.y );

				//const float w = h * 0.5f;
				//uemath::fvector2d draw_pos = { foot_pos.x - w / 2 ,foot_pos.y };

				//m_framework.get( )->current_canvas->box( draw_pos, { w, h }, Color, 1.0f );
			}

		}



	} while ( false );
}



