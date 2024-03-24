#pragma once 
#include "..\..\includes.hpp"

auto vars::c_vars::find_offset( 
	wchar_t* class_name, 
	wchar_t* member_name ) -> const std::uintptr_t
{
	auto current_obj = finals::u_object::find_object( 
		class_name,
		reinterpret_cast< finals::u_object* >( -1 )
	);

	if ( current_obj )
	{
		for ( auto property = *( uint64_t* ) ( current_obj + 0x78 ); property; property = *( uint64_t* ) ( property + 0x90 ) )
		{
			const auto type = *( ULONG64* ) ( property + 0x0 );

			if ( !stub::callstack<BOOL>( m_stub.get( )->gadget_rbx, IsBadReadPtr, ( const void* ) type, ( UINT_PTR ) 8 ) && type )
			{
				const auto property_fname = *( finals::fname* ) ( property + 0x98 );

				if ( property_fname.index == kismet_string_library::conv_string_to_name( member_name ).index )
				{
					return *( ULONG32* ) ( property + 0xBC );
				}
			}
		}
	}

	return 0;
}

const bool vars::c_vars::setup( ) {
	//auto find_obj_signature = pattern::find( 
	//	"56 48 81 EC ? ? ? ? 48 89 CE 48 8B 05 ? ? ? ? 48 31 E0 48 89 84 24 ? ? ? ? 83 7A 08 00 74 05 48 8B 02 EB 07",
	//	m_util->game_base
	//);

	//if ( !find_obj_signature ) {
	//	return 0;
	//}

	//m_util.get( )->find_obj = std::move( find_obj_signature );

	m_util.get( )->engine = (uintptr_t)u_object::find_object(
		OBFUSCATE_STR( L"EmbarkGameEngine_2147482621" ),
		reinterpret_cast<u_object*>( -1 )
	);

	m_util.get( )->font = (uintptr_t)u_object::find_object(
		OBFUSCATE_STR( L"DefaultRegularFont" ),
		reinterpret_cast<u_object*>( -1 )
	);

	finals::kismet::game_statics = u_object::find_object(
		OBFUSCATE_STR( L"Engine.Default__GameplayStatics" )
	);

	finals::kismet::math = u_object::find_object(
		OBFUSCATE_STR( L"Engine.Default__KismetMathLibrary" )
	);

	finals::kismet::string = u_object::find_object(
		OBFUSCATE_STR( L"Engine.Default__KismetStringLibrary" )
	);

	finals::kismet::system = u_object::find_object(
		OBFUSCATE_STR( L"Engine.Default__KismetSystemLibrary" )
	);

	if ( !finals::kismet::game_statics ||
		!finals::kismet::math ||
		!finals::kismet::string ||
		!finals::kismet::system ) {

		LOG( "failed to get some of the kismet classes.\n" );

		return 0;
	}

	m_nom.get( )->data.controller = find_offset(
		OBFUSCATE_STR( L"Engine.Pawn" ),
		OBFUSCATE_STR( L"Controller" )
	);

	m_nom.get( )->data.game_instance = find_offset(
		OBFUSCATE_STR( L"Engine.World" ),
		OBFUSCATE_STR( L"OwningGameInstance" )
	);

	m_nom.get( )->data.local_players = find_offset(
		OBFUSCATE_STR( L"Engine.GameInstance" ),
		OBFUSCATE_STR( L"LocalPlayers" )
	);

	m_nom.get( )->data.viewport = find_offset(
		OBFUSCATE_STR( L"Engine.Engine" ),
		OBFUSCATE_STR( L"GameViewport" )
	);

	m_nom.get( )->data.world = find_offset( 
		OBFUSCATE_STR( L"Engine.GameViewportClient" ),
		OBFUSCATE_STR( L"World" )
	);

	m_nom.get( )->data.game_state = find_offset(
		OBFUSCATE_STR( L"Engine.World" ),
		OBFUSCATE_STR( L"GameState" )
	);

	m_nom.get( )->data.mesh = find_offset(
		OBFUSCATE_STR( L"Engine.Character" ),
		OBFUSCATE_STR( L"Mesh" )
	);

	m_nom.get( )->data.level = find_offset(
		OBFUSCATE_STR( L"Engine.World" ),
		OBFUSCATE_STR( L"PersistentLevel" )
	);

	m_nom.get( )->data.local_controller = find_offset(
		OBFUSCATE_STR( L"Engine.LocalPlayer" ),
		OBFUSCATE_STR( L"PendingLevelPlayerControllerClass" )
	);

	m_nom.get( )->data.b_is_local_controller = find_offset(
		OBFUSCATE_STR( L"Engine.PlayerController" ),
		OBFUSCATE_STR( L"bIsLocalPlayerController" )
	);

	m_nom.get( )->data.pawn_private = find_offset(
		OBFUSCATE_STR( L"Engine.PlayerState" ),
		OBFUSCATE_STR( L"PawnPrivate" )
	);

	m_nom.get( )->data.player_array = find_offset(
		OBFUSCATE_STR( L"Engine.GameStateBase" ),
		OBFUSCATE_STR( L"PlayerArray" )
	);


	if ( !m_nom.get( )->data.viewport ||
		!m_nom.get( )->data.world ||
		!m_nom.get( )->data.game_state ||
		!m_nom.get( )->data.game_instance ||
		!m_nom.get( )->data.mesh ||
		!m_nom.get( )->data.level ||
		!m_nom.get( )->data.controller ||
		!m_nom.get( )->data.local_controller ||
		!m_nom.get( )->data.b_is_local_controller ||
		!m_nom.get( )->data.pawn_private ||
		!m_nom.get( )->data.player_array ) {

		LOG("failed to get some offsets.\n" );

		return 0;
	}

	m_nom.get( )->data.render_index = 5;

	return 1;
}