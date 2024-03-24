#pragma once 
namespace visual
{
	class c_visual
	{
	private:

		std::mutex m_mutex {};

	public:

		c_visual( );

		void render( );
	};

} inline auto m_visual = std::make_shared<visual::c_visual>( );