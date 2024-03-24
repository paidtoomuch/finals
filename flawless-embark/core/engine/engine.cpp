#pragma once 
#include "../includes.hpp"

auto uengine::c_engine::in_screen(
	uemath::fvector bone ) -> bool {

	if ( bone.x > 0 && bone.x < this->m_width && bone.y > 0 && bone.y < this->m_height ) {
		return true;
	}
	else {
		return false;
	}
}

auto uengine::c_engine::is_shot(
	uemath::fvector lur, uemath::fvector wl ) -> bool {

	if ( lur.x >= wl.x - 20 && lur.x <= wl.x + 20 && lur.y >= wl.y - 20 && lur.y <= wl.y + 20 && lur.z >= wl.z - 30 && lur.z <= wl.z + 30 )
		return true;
	else
		return false;

}

auto uengine::c_engine::get_cross_distance(
	double x1,
	double y1,
	double x2,
	double y2 ) -> double {

	return sqrtf( powf( ( x2 - x1 ), 2 ) + powf( ( y2 - y1 ), 2 ) );
}

auto uengine::ftransform::to_matrix( ) -> uengine::c_matrix {
	uengine::c_matrix matrix{};

	auto x2 = this->rotation.x * 2;
	auto y2 = this->rotation.y * 2;
	auto z2 = this->rotation.z * 2;

	auto xx2 = this->rotation.x * x2;
	auto yy2 = this->rotation.y * y2;
	auto zz2 = this->rotation.z * z2;

	auto yz2 = this->rotation.y * z2;
	auto wx2 = this->rotation.w * x2;

	auto xy2 = this->rotation.x * y2;
	auto wz2 = this->rotation.w * z2;

	auto xz2 = this->rotation.x * z2;
	auto wy2 = this->rotation.w * y2;

	matrix.x_plane.x = ( 1.0 - ( yy2 + zz2 ) ) * this->scale.x;
	matrix.x_plane.y = ( xy2 + wz2 ) * this->scale.x;
	matrix.x_plane.z = ( xz2 - wy2 ) * this->scale.x;

	matrix.y_plane.x = ( xy2 - wz2 ) * this->scale.y;
	matrix.y_plane.y = ( 1.0 - ( xx2 + zz2 ) ) * this->scale.y;
	matrix.y_plane.z = ( yz2 + wx2 ) * this->scale.y;

	matrix.z_plane.x = ( xz2 + wy2 ) * this->scale.z;
	matrix.z_plane.y = ( yz2 - wx2 ) * this->scale.z;
	matrix.z_plane.z = ( 1.0 - ( xx2 + yy2 ) ) * this->scale.z;

	matrix.w_plane.x = this->translation.x;
	matrix.w_plane.y = this->translation.y;
	matrix.w_plane.z = this->translation.z;

	matrix.w_plane.w = 1.0;

	return matrix;
}

auto uengine::c_matrix::to_rotation_matrix(
	uemath::frotator rotation ) -> uengine::c_matrix
{

	auto rad_pitch = ( rotation.pitch * double( std::numbers::pi ) / 180.f );
	auto rad_yaw = ( rotation.yaw * double( std::numbers::pi ) / 180.f );
	auto rad_roll = ( rotation.roll * double( std::numbers::pi ) / 180.f );

	auto sin_pitch = sinf( rad_pitch );
	auto cos_pitch = cosf( rad_pitch );

	auto sin_yaw = sinf( rad_yaw );
	auto cos_yaw = cosf( rad_yaw );

	auto sin_roll = sinf( rad_roll );
	auto cos_roll = cosf( rad_roll );

	uengine::c_matrix matrix{};

	matrix.x_plane.x = cos_pitch * cos_yaw;
	matrix.x_plane.y = cos_pitch * sin_yaw;
	matrix.x_plane.z = sin_pitch;

	matrix.y_plane.x = sin_roll * sin_pitch * cos_yaw - cos_roll * sin_yaw;
	matrix.y_plane.y = sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
	matrix.y_plane.z = -sin_roll * cos_pitch;

	matrix.z_plane.x = -( cos_roll * sin_pitch * cos_yaw + sin_roll * sin_yaw );
	matrix.z_plane.y = cos_yaw * sin_roll - cos_roll * sin_pitch * sin_yaw;
	matrix.z_plane.z = cos_roll * cos_pitch;

	matrix.w_plane.w = 0x1;

	return matrix;
}

auto uengine::c_matrix::to_multiplication(
	uengine::c_matrix m_matrix ) -> uengine::c_matrix {

	uengine::c_matrix matrix{};

	matrix.w_plane.x = (
		this->w_plane.x * m_matrix.x_plane.x +
		this->w_plane.y * m_matrix.y_plane.x +
		this->w_plane.z * m_matrix.z_plane.x +
		this->w_plane.w * m_matrix.w_plane.x
		);

	matrix.w_plane.y = (
		this->w_plane.x * m_matrix.x_plane.y +
		this->w_plane.y * m_matrix.y_plane.y +
		this->w_plane.z * m_matrix.z_plane.y +
		this->w_plane.w * m_matrix.w_plane.y
		);

	matrix.w_plane.z = (
		this->w_plane.x * m_matrix.x_plane.z +
		this->w_plane.y * m_matrix.y_plane.z +
		this->w_plane.z * m_matrix.z_plane.z +
		this->w_plane.w * m_matrix.w_plane.z
		);

	matrix.w_plane.w = (
		this->w_plane.x * m_matrix.x_plane.w +
		this->w_plane.y * m_matrix.y_plane.w +
		this->w_plane.z * m_matrix.z_plane.w +
		this->w_plane.w * m_matrix.w_plane.w
		);

	return matrix;
}
