#pragma once 
#include "../../core/includes.hpp"

auto framework::c_framework::get_color( int a,int r,int g,int b ) -> uemath::flinearcolor {

	return uemath::flinearcolor { 
		static_cast< float >( a ),
		static_cast< float >( r ),
		static_cast< float >( g ),
		static_cast< float >( b )
	};

}

void framework::c_framework::line( uemath::fvector2d x, uemath::fvector2d y, uemath::flinearcolor color, float thickness ) {
	current_canvas->line( x, y, thickness, color );
}

void framework::c_framework::rect( uemath::fvector2d position, uemath::fvector2d size, uemath::flinearcolor color, float thickness ) {
	line( uemath::fvector2d( position.x, position.y ), uemath::fvector2d( position.x + size.x, position.y ), color, thickness );
	line( uemath::fvector2d( position.x + size.x, position.y ), uemath::fvector2d( position.x + size.x, position.y + size.y ), color, thickness );
	line( uemath::fvector2d( position.x + size.x, position.y + size.y ), uemath::fvector2d( position.x, position.y + size.y ), color, thickness );
	line( uemath::fvector2d( position.x, position.y + size.y ), uemath::fvector2d( position.x, position.y ), color, thickness );
}

void framework::c_framework::filled_rect( uemath::fvector2d position, uemath::fvector2d size ,uemath::flinearcolor color ) {
	for ( int i = 0; i < size.y; i++ )
	{
		line( uemath::fvector2d( position.x, position.y + i ), uemath::fvector2d( position.x + size.x, position.y + i ), color, 1.f );
	}
}

void framework::c_framework::circle( uemath::fvector2d position, uemath::flinearcolor color, double radius, double segments ) {

	double m_pi = 3.1415927;

	double step = m_pi * 2.0 / segments;
	int count = 0;
	uemath::fvector2d vectors[128];
	for ( double deg = 0; deg < m_pi * 2; deg += step )
	{
		double x_1 = radius * finals::kismet_math_library::cos( deg ) + position.x;
		double y_1 = radius * finals::kismet_math_library::sin( deg ) + position.y;
		double x_2 = radius * finals::kismet_math_library::cos( deg + step ) + position.x;
		double y_2 = radius * finals::kismet_math_library::sin( deg + step ) + position.y;

		vectors[count].x = x_1;
		vectors[count].y = y_1;
		vectors[count + 1].x = x_2;
		vectors[count + 1].y = y_2;

		line( uemath::fvector2d( vectors[count].x, vectors[count].y ), uemath::fvector2d( x_2, y_2 ), color, 1.f );
	}

}

void framework::c_framework::text(
	const fstring content, 
	uemath::fvector2d screen, 
	uemath::flinearcolor color, 
	bool center, 
	bool outlined, 
	double scale ) {
	current_canvas->text( content, screen, uemath::fvector2d( scale, scale ), color, scale, uemath::flinearcolor(), uemath::fvector2d(), center ? true : false, center ? true : false, outlined, uemath::flinearcolor( 0.f, 0.f, 0.f, 1.f ) );
}

