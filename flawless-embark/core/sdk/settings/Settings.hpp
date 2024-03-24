#pragma once

class c_instance
{
public:
	c_instance( ) {}
	c_instance( const std::uintptr_t& inst )
	{
		this->instance = inst;
	}
	~c_instance( ) {}

protected:
	std::uintptr_t instance = 0;


public:
	operator std::uintptr_t( ) const
	{
		return this->instance;
	}
};

namespace settings
{
	class c_settings : public c_instance
	{

	public: 

		using c_instance::c_instance;

		void load( );
		void save( );
		void reset( );

		struct {
			int aim_keycode;
			int target_bone;
			bool enabled;
			float fov_c; 
			float smooth;
			float accuracy_rate;
			bool ignore_team;
			bool ignore_knocked;
			bool ignore_invisible;
		} aim;

		struct {
			bool no_double_pump_cooldown;
			bool instant_weapon_animation;
			bool no_sprint_to_shoot_cooldown;
			bool first_person;
			bool no_recoil;
			bool no_spread;
		} exploits;

		struct {
			struct {
				bool enabled;
				bool prediction;
				float weapon_fov;
			} smg;

			struct {
				bool enabled;
				bool prediction;
				float weapon_fov;
			} ars;

			struct {
				bool enabled;
				float weapon_fov;
			} shotgun;

			struct {
				bool enabled;
				bool prediction;
				float weapon_fov;
			} pistol;

			struct {
				bool enabled;
				bool prediction;
				float weapon_fov;
			} snipers;

		} weapon_config;
		
		struct {
			bool esp;
			
			bool box;
			bool corner;
			bool username;
			bool distance;
			bool snapline;
			bool weapon;
			bool skeleton;
			bool view_angle;
			
			bool ignore_team;
			bool ignore_knocked;

		} visuals;

		struct {
			bool enabled;
			float width;
			float height;
			bool show_team;
		} radar;

		struct {
			bool weapons;
			bool items;
			bool vehicles;
			bool chests;
			bool ammo;

			bool smart_loot;
			bool priot_tier;
			bool prior_ammo;
			bool prior_health;
			bool prior_shield;
			
			float weapons_render_distance = 50.0f;
			float items_render_distance = 50.0f;
			float vehicles_render_distance = 150.0f;
			float chests_render_distance = 50.0f;
			float ammo_render_distance = 50.0f;
		} loot;

		struct {

			bool fov;
			bool fps;
			bool menu;
			bool trigger_bot;
			bool crosshair;

			bool kill;

			float custom_delay;
			float maximum_distance;
			bool has_clicked;
			std::chrono::steady_clock::time_point tb_begin;
			std::chrono::steady_clock::time_point tb_end;
			int tb_time_since;

		} misc;

		struct {

		} keybind;
	};

}
inline std::shared_ptr<settings::c_settings> m_setting = std::make_shared<settings::c_settings>( );

namespace offsets
{
	class c_offsets : public c_instance
	{
	public:
		using c_instance::c_instance;

		struct {
			std::int32_t viewport;
			std::int32_t world;
			std::int32_t local_controller;
			std::int32_t controller;
			std::int32_t game_state;
			std::int32_t game_instance;
			std::int32_t mesh;
			std::int32_t level;
			std::int32_t b_is_local_controller;
			std::int32_t pawn_private;
			std::int32_t player_array;
			std::int32_t local_players;

			int render_index;

		} data;
	};
}
inline std::shared_ptr<offsets::c_offsets> m_nom = std::make_shared<offsets::c_offsets>( );

// nom - numerous offsets managers
