#pragma once 
namespace aimbot
{
	class c_aimbot
	{
		std::mutex m_mutex {};

		std::unique_ptr<settings::c_settings>	m_setting;
		std::unique_ptr<uengine::c_engine>		m_engine;

	public:

		//c_aimbot(  );
		//c_aimbot( ) = default;
		c_aimbot( );

		uemath::fvector2d calculate_angle( uemath::fvector angle );

		void tick( );
	};
}