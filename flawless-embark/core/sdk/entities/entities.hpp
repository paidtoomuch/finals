#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#define current_class reinterpret_cast<uint64_t>(this )

#define declare_member(type, name, offset) type name() { return mem::read<type>((uintptr_t)(current_class + offset)); } 
#define declare_member_bit(Bit, Name, Offset) bool Name( ) { return bool( mem::read<char>(current_class + Offset) & (1 << Bit)); }

namespace finals {

	//size: 0x10
	template <class type>
	struct tarray
	{
		tarray( ) { this->data = nullptr; this->count = this->max = std::uint32_t( 0 ); };

		type* data;
		std::uint32_t count, max;

		type& operator[]( int i ) { return this->data[i]; };

		int size( ) { return this->count; }

		bool valid( int i ) { return bool( i < this->count ); }
	};

	struct fstring : tarray<wchar_t>
	{
		inline fstring( ) {
		};

		fstring( const wchar_t* other )
		{
			this->max = this->count = *other ? static_cast< int >( customs::wcslen( other ) ) + 1 : 0;

			if ( this->count )
				this->data = const_cast< wchar_t* >( other );
		};

		wchar_t* c_str( )
		{
			return this->data;
		}

		bool valid( )
		{
			return this->data != nullptr;
		}
	};


	struct fname
	{
		fname( ) : index( ) {}
		fname( std::uint32_t index ) : index( index ) {}

		operator bool( ) { return bool( this->index ); }
		friend bool operator	== ( const fname& a, const fname& b ) { return a.index == b.index; }
		friend bool operator	!= ( const fname& a, const fname& b ) { return !( a == b ); }

		std::uint32_t index;
	};

	// Class CoreUObject.Object
	class u_object
	{
	public:
		static auto find_object( const wchar_t* name, u_object* outer = nullptr ) -> u_object*
		{
			auto name_to_fstring = (fstring)( name );

			return stub::callstack<u_object*>( m_stub.get( )->gadget_rbx, reinterpret_cast< u_object * ( * )( u_object*, fstring* ) >( m_util.get( )->game_base + 0x1902760 ), outer, &name_to_fstring );
		}

		auto process_event( u_object* function, void* params ) -> void
		{
			static int pe_idx = 76;

			//if ( !pe_idx ) {
			//	const auto pe_enc_sig = pattern::find( 
			//		"48 81 EC ? ? ? ? 48 89 CE 48 8B 05 ? ? ? ? 48 31 E0 48 89 84 24 ? ? ? ? 83 7A 08 00 74 05 48 8B 02 EB 07", 
			//		m_util.get( )->game_base 
			//	);

			//	if ( !pe_enc_sig ) {
			//		LOG ( "failed to resolve pe signature, reverting call.\n" );
			//		return;
			//	}

			//	pe_idx = *( int32_t* ) ( pe_enc_sig + 3 + 2 ) / 8;

			//	if ( !pe_idx ) {
			//		LOG( "failed to resolve pe index, reverting to 82.\n" );
			//		pe_idx = 82;
			//	}
			//}

			auto vtable = *reinterpret_cast< void*** >( this );
			auto pecall = reinterpret_cast< void( * )( u_object*, void*, void* ) >( vtable[pe_idx] );

			return stub::callstack<void>( m_stub.get( )->gadget_rbx, pecall, this, ( void* ) function, params );
		}

	public:

		declare_member( 
			fname, 
			name_private,
			0x18 
		);
	};

	// Class Engine.Canvas
	class u_canvas : public u_object
	{
	public:
		auto line( uemath::fvector2d screen_position_a, uemath::fvector2d screen_position_b, float thickness, uemath::flinearcolor color ) -> void
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Canvas.K2_DrawLine" ) );

			struct
			{
				uemath::fvector2d arg1;
				uemath::fvector2d arg2;
				float arg3;
				uemath::flinearcolor arg4;
			} parameters = { screen_position_a, screen_position_b, thickness, color };

			this->process_event( object, &parameters );
		}

		auto text( fstring text, uemath::fvector2d screen_position, uemath::fvector2d scale, uemath::flinearcolor color, float kerning, uemath::flinearcolor shadow_color, uemath::fvector2d shadow_offset, bool center_x, bool center_y, bool outlined, uemath::flinearcolor outline_color )
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Canvas.K2_DrawText" ) );

			struct
			{
				u_object* arg1;
				fstring arg2;
				uemath::fvector2d arg3;
				uemath::fvector2d arg4;
				uemath::flinearcolor arg5;
				float arg6;
				uemath::flinearcolor arg7;
				uemath::fvector2d arg8;
				bool arg9;
				bool arg10;
				bool arg11;
				uemath::flinearcolor arg12;
			} parameters = { ( u_object* ) m_util.get( )->font, text, screen_position, scale, color, kerning, shadow_color, shadow_offset, center_x, center_y, outlined, outline_color };

			this->process_event( object, &parameters );
		}

		auto box( uemath::fvector2d position, uemath::fvector2d size, uemath::flinearcolor render_color, float thickness ) 
		{
			this->line( uemath::fvector2d( position.x, position.y ), uemath::fvector2d( position.x + size.x, position.y ), thickness, render_color );
			this->line( uemath::fvector2d( position.x + size.x, position.y ), uemath::fvector2d( position.x + size.x, position.y + size.y ), thickness, render_color );
			this->line( uemath::fvector2d( position.x + size.x, position.y + size.y ), uemath::fvector2d( position.x, position.y + size.y ), thickness, render_color );
			this->line( uemath::fvector2d( position.x, position.y + size.y ), uemath::fvector2d( position.x, position.y ), thickness, render_color );
		}

	};

	class u_actor_component : public u_object
	{
	public:
	};

	class u_scene_component : public u_actor_component
	{
	public:
	};

	class u_primitive_component : public u_scene_component
	{
	public:
	};

	class u_mesh_component : public u_primitive_component
	{
	public:
	};

	class u_skinned_mesh_component : public u_mesh_component
	{
	public:
		auto get_socket_name( int32_t num ) -> fname
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( ( L"Engine.SkinnedMeshComponent.GetBoneName" ) );

			struct {
				int32_t bone_index;
				fname return_value;
			} params;

			params.bone_index = num;

			this->process_event( object, &params );

			return params.return_value;
		}

		auto get_socket_location( fname name ) -> uemath::fvector
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( ( L"Engine.SceneComponent.GetSocketLocation" ) );

			struct {
				fname socket_fname;
				uemath::fvector return_value;
			} params;

			params.socket_fname = name;

			this->process_event( object, &params );

			return params.return_value;
		}

		auto get_num_bones( ) -> int32_t
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.SkinnedMeshComponent.GetNumBones" ) );

			struct {
				int32_t return_value;
			} params;

			this->process_event( object, &params );

			return params.return_value;
		}
	};

	class u_skeletal_mesh_component : public u_skinned_mesh_component
	{
	public:

	};


	class a_actor : public u_object
	{
	public:
		auto get_location( ) -> uemath::fvector
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Actor.K2_GetActorLocation" ) );

			struct {
				uemath::fvector return_value;
			} params;

			this->process_event( object, &params );

			return params.return_value;
		}

		auto get_actor_bounds( bool b_only_colliding_components, struct uemath::fvector& origin, struct uemath::fvector& box_extent, bool b_include_from_child_actors )
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Actor.GetActorBounds" ) );

			struct
			{
				bool b_only_colliding_components;
				uemath::fvector origin;
				uemath::fvector box_extent;
				bool b_include_from_child_actors;
			} params;

			this->process_event( object, &params );

			origin = params.origin;
			box_extent = params.box_extent;
		}

		auto get_rotation( ) -> uemath::fvector
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Actor.K2_GetActorRotation" ) );

			struct {
				uemath::fvector return_value;
			} params;

			this->process_event( object, &params );

			return params.return_value;
		}
	};

	class a_controller : public a_actor
	{
	public:
		auto get_pawn( ) -> u_object*
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Controller.K2_GetPawn" ) );

			struct {
				u_object* return_value;
			} params;

			this->process_event( object, &params );

			return params.return_value;
		}

		auto line_of_sight_to( a_actor* other ) -> bool
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.Controller.LineOfSightTo" ) );

			struct {
				a_actor* other;
				uemath::fvector view_point;
				bool b_alternate_checks;
				bool return_value;
			} params;

			params.other = other;
			params.view_point = uemath::fvector{ 0, 0, 0 };
			params.b_alternate_checks = FALSE;

			this->process_event( object, &params );

			return params.return_value;
		}
	};

	class a_player_controller : public a_controller
	{
	public:
		declare_member( 
			bool, 
			bIsLocalPlayerController, 
			m_nom.get( )->data.b_is_local_controller 
		);

	public:
		auto project_world_location_to_screen( uemath::fvector world_location, uemath::fvector2d* screen_location ) -> bool
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.PlayerController.ProjectWorldLocationToScreen" ) );

			struct {
				uemath::fvector world_location;
				uemath::fvector2d screen_location;
				bool return_value;
			} params = { world_location, uemath::fvector2d( ), true };

			this->process_event( object, &params );

			*screen_location = params.screen_location;

			return params.return_value;
		}
	};

	class a_pawn : public a_actor
	{
	public:
		declare_member( 
			u_skeletal_mesh_component*, 
			mesh, 
			m_nom.get( )->data.mesh 
		);

		declare_member(
			a_player_controller*,
			controller,
			m_nom.get( )->data.controller
		);
	};

	class a_player_state : public u_object
	{
	public:
		declare_member( 
			a_pawn*,
			pawn_private,
			m_nom.get( )->data.pawn_private 
		);

	public:
		auto get_player_name( ) -> fstring
		{
			static u_object* object;
			if ( !object )
				object = u_object::find_object( OBFUSCATE_STR( L"Engine.PlayerState.GetPlayerName" ) );

			struct {
				fstring return_value;
			} params;

			this->process_event( object, &params );

			return params.return_value;
		}
	};

	class u_level : public u_object
	{
	public:
	};

	class a_character : public a_pawn
	{
	public:
	};

	class a_game_state_base : public u_object
	{
	public:
		declare_member( 
			tarray<a_player_state*>, 
			player_array,  
			m_nom.get( )->data.player_array
		);
	};

	class u_local_player : public u_object
	{
	public:
		declare_member(
			a_player_controller*,
			controller,
			m_nom.get( )->data.local_controller
		);
	};

	class u_game_instance : public u_object
	{
	public:
		declare_member(
			tarray<u_local_player*>,
			local_player_array,
			m_nom.get( )->data.local_players
		);
	};

	class u_world : public u_object
	{
	public:
		declare_member(
			u_game_instance*,
			game_instance,
			m_nom.get( )->data.game_instance
		);

		declare_member(
			a_game_state_base*,
			game_state,
			m_nom.get( )->data.game_state
		);

		declare_member(
			u_level*,
			persistent_level,
			m_nom.get( )->data.level
		);
	};

	class u_game_viewport_client : public u_object
	{
	public:
		declare_member(
			u_world*,
			world,
			m_nom.get( )->data.world
		);
	};

	namespace kismet
	{
		static u_object* system;
		static u_object* game_statics;
		static u_object* string;
		static u_object* math;
	}

	class kismet_system_library {
	public:
		static fstring get_object_name( u_object* object )
		{
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetSystemLibrary.GetObjectName" ) );
			
			struct {
				u_object* object;
				fstring return_value;
			} params = { object };

			kismet::system->process_event( member, &params );

			return params.return_value;
		}

		static u_object* get_outer_object( u_object* object ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetSystemLibrary.GetOuterObject" ) );

			struct {
				u_object* object;
				u_object* return_value;
			} params = { object };

			kismet::system->process_event( member, &params );

			return params.return_value;
		}
	};

	class gameplay_statics
	{
	public:
		static u_game_instance* get_game_instance( u_object* world_context_object ) 
		{
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.GameplayStatics.GetGameInstance" ) );

			struct {
				u_object* world_context_object;
				u_game_instance* return_value;
			} params = { world_context_object };

			kismet::game_statics->process_event( member, &params );

			return params.return_value;
		}

		static a_player_controller* get_player_controller_from_id( u_object* world_context_object )
		{
			static u_object* member;
			if ( !member )
				member = u_object::find_object( L"Engine.GameplayStatics.GetPlayerControllerFromID" );

			struct {
				u_object* world_context_object;
				std::int32_t player_index;
				a_player_controller* return_value;
			} params = { world_context_object, 0 };

			kismet::game_statics->process_event( member, &params );

			return params.return_value;
		}

		static tarray<u_object*> get_all_actors_of_class( u_object* world, u_object* actor_class ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.GameplayStatics.GetAllActorsOfClass" ) );

			struct
			{
				u_object* world;
				u_object* actor_class;
				tarray<u_object*> return_value;
			} params = { world, actor_class };


			kismet::game_statics->process_event( member, &params );

			return params.return_value;
		}
	};

	class kismet_string_library
	{
	public:
		static fname conv_string_to_name( fstring in_string ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetStringLibrary.Conv_StringToName" ) );

			struct {
				fstring in_string;
				fname return_value;
			} params = { in_string };

			kismet::string->process_event( member, &params );

			return params.return_value;
		}

		static bool contains( fstring search_in, fstring sub_string ) {
			static u_object* member;
			if ( !member ) 
				member =  u_object::find_object( OBFUSCATE_STR( L"Engine.KismetStringLibrary.Contains" ) );

			struct {
				fstring search_in;
				fstring sub_string;
				bool use_case;
				bool search_from_end;
				bool return_value;
			} params = { search_in, sub_string, false, false };

			kismet::string->process_event( member, &params );

			return params.return_value;
		}

		static fstring concat_strstr( fstring a, fstring b ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetStringLibrary.Concat_StrStr" ) );

			struct {
				fstring a;
				fstring b;
				fstring return_value;
			} params = { a, b };

			kismet::string->process_event( member, &params );

			return params.return_value;
		}

		static fstring build_string_double( fstring append_to, fstring prefix, double in_double, fstring suffix ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetStringLibrary.BuildString_Double" ) );

			struct {
				fstring append_to;
				fstring prefix;
				double in_double;
				fstring suffix;
				fstring return_value;
			} params = { append_to, prefix, in_double, suffix };

			kismet::string->process_event( member, &params );

			return params.return_value;
		}

		static fstring build_string_int( fstring append_to, fstring prefix, int32_t in_int, fstring suffix ) {
			static u_object* member;
			if ( !member ) 
				member =  u_object::find_object( OBFUSCATE_STR( L"Engine.KismetStringLibrary.BuildString_Int" ) );

			struct {
				fstring append_to;
				fstring prefix;
				int32_t in_int;
				fstring suffix;
				fstring return_value;
			} params = { append_to, prefix, in_int, suffix };

			kismet::string->process_event( member, &params );

			return params.return_value;
		}
	};


	class kismet_math_library {
	public:

		static float sin( float a ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.sin" ) );

			struct {
				float a;
				float return_value;
			} params = { a };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static float cos( float a ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.tan" ) );

			struct {
				float a;
				float return_value;
			} params = { a };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static float tan( float a ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.tan" ) );

			struct {
				float a;
				float return_value;
			} params = { a };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static float atan2( float x, float y ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.Atan2" ) );

			struct {
				float x;
				float y;
				float return_value;
			} params = { x, y };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static uemath::fvector r_interp_to( uemath::fvector current, uemath::fvector target, float delta_time, float speed ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.RInterpTo" ) );

			struct {
				uemath::fvector current;
				uemath::fvector target;
				float delta_time;
				float speed;
				uemath::fvector return_value;
			} params = { current, target, delta_time, speed };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static uemath::fvector r_interp_to_constant( uemath::fvector current, uemath::fvector target, float delta_time, float speed ) {
			static u_object* member;
			if ( !member ) 
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.RInterpTo_Constant" ) );

			struct {
				uemath::fvector current;
				uemath::fvector target;
				float delta_time;
				float speed;
				uemath::fvector return_value;
			} params = { current, target, delta_time, speed };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static float vector_distance( uemath::fvector v1, uemath::fvector v2 ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.Vector_Distance" ) );

			struct {
				uemath::fvector current;
				uemath::fvector target;
				float return_value;
			} params = { v1, v2 };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static float distance_2d( uemath::fvector2d v1, uemath::fvector2d v2 ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.Distance2D" ) );

			struct {
				uemath::fvector2d current;
				uemath::fvector2d target;
				float return_value;
			} params = { v1, v2 };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}

		static uemath::fvector find_look_at_rotation( uemath::fvector start, uemath::fvector target ) {
			static u_object* member;
			if ( !member )
				member = u_object::find_object( OBFUSCATE_STR( L"Engine.KismetMathLibrary.FindLookAtRotation" ) );

			struct {
				uemath::fvector start;
				uemath::fvector target;
				uemath::fvector return_value;
			} params = { start, target };

			kismet::math->process_event( member, &params );

			return params.return_value;
		}
	};
	
	class c_item {
	public:
		uemath::fvector		m_item_base;
		float               m_distance;
		std::string			m_name;
	};

	class c_actor {
	public:
		uemath::fvector		m_actor_base;
		uemath::fvector     m_actor_head;
		std::string			m_username;
		float               m_distance;
		bool				m_is_bot;
		bool				m_is_visible;
		bool				m_is_knocked;
		bool				m_is_dying;
		bool				m_is_target;
	};
}
//
#endif // !gaurd