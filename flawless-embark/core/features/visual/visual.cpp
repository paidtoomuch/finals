#pragma once 
#include "../../includes.hpp"

visual::c_visual::c_visual( )
{
	m_setting.get( )->load( );
}

auto visual::c_visual::render( ) -> void
{
	if ( !m_framework.get( ) )
		return;

	std::unique_lock<std::mutex> lock( this->m_mutex );

	for ( auto i = 0; i < m_entity->size( ); i++ )
	{
		auto entity = m_entity->m_actors[i];
		
	}


	lock.unlock( );
}
