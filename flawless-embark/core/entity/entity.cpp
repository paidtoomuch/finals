#pragma once 
#include "../includes.hpp"

auto timer_stop = std::chrono::high_resolution_clock::now( );

void entity::c_entity::tick( )
{
	
}

auto entity::c_entity::update( ) -> void
{

	this->m_actors.swap( m_players );
}