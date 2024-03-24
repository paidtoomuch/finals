#pragma once 
#include "../../includes.hpp"

auto clock_old = std::chrono::high_resolution_clock::now( );

aimbot::c_aimbot::c_aimbot( )
{
	this->m_engine		= std::make_unique<uengine::c_engine>( );
	this->m_setting		= std::make_unique<settings::c_settings>( );

	this->m_setting.get( )->load( );

}

bool PositiveCurve = false;
auto aimbot::c_aimbot::calculate_angle( uemath::fvector angle ) -> uemath::fvector2d
{
	uemath::fvector center(
		m_engine.get( )->m_width_2,
		m_engine.get( )->m_height_2, 0
	);

	uemath::fvector target( 0, 0, 0 );

	// Randomly swap the curve
	if ( rand( ) % 1550 == 0 )
	{
		PositiveCurve = !PositiveCurve;
	}

	if ( angle.x != 0 || angle.y != 0 )
	{
		if ( angle.x > center.x )
			target.x = -(center.x - angle.x);

		if ( angle.x < center.x )
			target.x = angle.x - center.x;

		if ( angle.y > center.y )
			target.y = -(center.y - angle.y);

		if ( angle.y < center.y )
			target.y = angle.y - center.y;
	}

	// Rage
	if ( m_setting.get( )->aim.smooth <= 2.f )
	{
		target.x *= 2.5;
		target.y *= 2.5;
	}

	float dx = target.x / m_setting.get( )->aim.smooth;
	if ( dx > 0.24 && dx < 1 ) dx = 1;
	if ( dx < -0.24 && dx > -1 ) dx = -1;

	float dy = target.y / m_setting.get( )->aim.smooth;
	if ( dy > 0.24 && dy < 1 ) dy = 1;
	if ( dy < -0.24 && dy > -1 ) dy = -1;

	if ( m_setting.get( )->aim.smooth <= 2.f )
	{
		if ( dx > 256 )
		{
			dx = 256;
		}
		else if ( dx < -256 )
		{
			dx = -256;
		}

		if ( dy > 256 )
		{
			dy = 256;
		}
		else if ( dy < -256 )
		{
			dy = -256;
		}
	}
	else
	{
		if ( dx > 68 )
		{
			dx = 68;
		}
		else if ( dx < -68 )
		{
			dx = -68;
		}

		if ( dy > 68 )
		{
			dy = 68;
		}
		else if ( dy < -68 )
		{
			dy = -68;
		}
	}

	if ( PositiveCurve )
	{
		int absvalx = abs( dx );
		int absvaly = abs( dy );

		if ( absvalx >= 32 )
		{
			dx = dx + rand( ) % 21;
		}
		else if ( absvalx > 16 )
		{
			dx = dx + rand( ) % 8;
		}
		else if ( absvalx > 8 )
		{
			dx = dx + rand( ) % 4;
		}
		else if ( absvalx > 3 )
		{
			dx = dx + rand( ) % 2;
		}
		else if ( absvalx > 1 )
		{
			dx = dx + rand( ) % 1;
		}

		if ( absvaly >= 32 )
		{
			dy = dy + rand( ) % 21;
		}
		else if ( absvaly > 16 )
		{
			dy = dy + rand( ) % 8;
		}
		else if ( absvaly > 8 )
		{
			dy = dy + rand( ) % 4;
		}
		else if ( absvaly > 3 )
		{
			dy = dy + rand( ) % 2;
		}
		else if ( absvaly > 1 )
		{
			dy = dy + rand( ) % 1;
		}
	}
	else
	{
		int absvalx = abs( dx );
		int absvaly = abs( dy );

		if ( absvalx >= 32 )
		{
			dx = dx - rand( ) % 21;
		}
		else if ( absvalx > 16 )
		{
			dx = dx - rand( ) % 8;
		}
		else if ( absvalx > 8 )
		{
			dx = dx - rand( ) % 4;
		}
		else if ( absvalx > 3 )
		{
			dx = dx - rand( ) % 2;
		}
		else if ( absvalx > 1 )
		{
			dx = dx - rand( ) % 1;
		}

		if ( absvaly >= 32 )
		{
			dy = dy - rand( ) % 21;
		}
		else if ( absvaly > 16 )
		{
			dy = dy - rand( ) % 8;
		}
		else if ( absvaly > 8 )
		{
			dy = dy - rand( ) % 4;
		}
		else if ( absvaly > 3 )
		{
			dy = dy - rand( ) % 2;
		}
		else if ( absvaly > 1 )
		{
			dy = dy - rand( ) % 1;
		}
	}

	return uemath::fvector2d( dx, dy );
}

auto aimbot::c_aimbot::tick( ) -> void
{
	//srand( time( 0 ) );

	//while ( true ) {

	//	constexpr auto update_time = 1;

	//	const auto time_now = std::chrono::high_resolution_clock::now( );
	//	const auto count = std::chrono::duration_cast< std::chrono::milliseconds >(
	//		time_now - clock_old
	//		).count( );

	//	if ( count > update_time )
	//	{
	//		std::unique_lock<std::mutex> mutex( this->m_mutex );

	//		//entity::c_actor m_actor { };
	//		//this->get_actors( m_actor );

	//		std::uintptr_t closest_mesh = 0;
	//		auto target_distance = flt_max;

	//		for ( auto& cache : entity::actor_array ) {

	//			if ( m_setting.get( )->misc.trigger_bot ) {

	//				auto is_held_key = (GetKeyState( VK_RBUTTON ) & 0x8000);
	//				if ( is_held_key ) {

	//					if ( is_shot( vm->read<fvector>( local_controller + 0x2530 ), head_bone ) ) {

	//						//if ( distance <= 20 ) {

	//						if ( m_setting.get( )->misc.has_clicked ) {

	//							m_setting.get( )->misc.tb_begin = std::chrono::steady_clock::now( );
	//							m_setting.get( )->misc.has_clicked = 0;

	//						}
	//						m_setting.get( )->misc.tb_end = std::chrono::steady_clock::now( );
	//						m_setting.get( )->misc.tb_time_since = std::chrono::duration_cast< std::chrono::milliseconds >(m_setting.get( )->misc.tb_end - m_setting.get( )->misc.tb_begin).count( );

	//						if ( m_setting.get( )->misc.tb_time_since >= m_setting.get( )->misc.custom_delay ) {

	//							// 1 = left down, 2 = left up, 4 = right down, 8 = right up, 16 = mouse down, 32 = mouse up
	//							this->dd_btn( 1 );
	//							this->dd_btn( 2 );
	//							m_setting.get( )->misc.has_clicked = 1;

	//						}
	//						//}

	//					}

	//				}

	//			}
	//			
	//			if ( m_setting.get( )->aim.mouse ) {

	//				auto dx = cache.m_actor_head.x - (m_engine.get( )->m_width_2);
	//				auto dy = cache.m_actor_head.y - (m_engine.get( )->m_height_2);
	//				auto dist = sqrtf( dx * dx + dy * dy );

	//				if ( dist < m_setting.get( )->aim.fov_c
	//					&& dist < target_distance ) {

	//					target_distance = dist;
	//					closest_mesh = cache.m_mesh;

	//				}

	//			}

	//		}

	//		if ( closest_mesh &&
	//			m_setting.get( )->aim.mouse ) {

	//			auto bone = m_engine.get( )->get_bone(
	//				closest_mesh,
	//				109
	//			);

	//			auto hitbox = m_engine.get( )->world_to_screen( bone );
	//			if ( !m_engine.get( )->in_screen( hitbox ) )
	//				return;

	//			auto cross_hair = m_engine.get( )->get_cross_distance(
	//				hitbox.x,
	//				hitbox.y,
	//				m_engine.get( )->m_width_2,
	//				m_engine.get( )->m_height_2
	//			);

	//			if ( cross_hair <= m_setting.get( )->aim.fov_c )
	//			{

	//				auto is_held = (GetKeyState( 0x02 ) & 0x8000);
	//				if ( is_held ) {

	//					this->aim_at_target(
	//						hitbox.x,
	//						hitbox.y
	//					);

	//				}

	//			}

	//		}
	//		else {
	//			closest_mesh = 0;
	//			target_distance = flt_max;
	//		}

	//		mutex.unlock( );
	//	}
	//	else {

	//		std::this_thread::sleep_for(
	//			std::chrono::milliseconds(
	//				update_time - count
	//			) );
	//	}
	//}
}