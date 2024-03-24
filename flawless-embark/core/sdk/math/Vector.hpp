#pragma once 
#include <cassert>

constexpr auto flt_max = 3.402823466e+38F; // max value
constexpr auto flt_min = 1.175494351e-38F; // min normalized positive value

namespace uemath
{
    //size: 0x18
    struct fvector
    {
        fvector( ) : x( 0 ), y( 0 ), z( 0 ) {}
        fvector( double x, double y, double z ) : x( x ), y( y ), z( z ) {}

        fvector operator		+ ( const fvector& other ) const { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
        fvector operator		- ( const fvector& other ) const { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
        fvector operator		* ( double offset ) const { return { this->x * offset, this->y * offset, this->z * offset }; }
        fvector operator		/ ( double offset ) const { return { this->x / offset, this->y / offset, this->z / offset }; }

        fvector& operator		*= ( const double other ) { this->x *= other; this->y *= other; this->z *= other; return *this; }
        fvector& operator		/= ( const double other ) { this->x /= other; this->y /= other; this->z /= other; return *this; }

        fvector& operator		=  ( const fvector& other ) { this->x = other.x; this->y = other.y; this->z = other.z; return *this; }
        fvector& operator		+= ( const fvector& other ) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
        fvector& operator		-= ( const fvector& other ) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
        fvector& operator		*= ( const fvector& other ) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
        fvector& operator		/= ( const fvector& other ) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

        operator bool( ) { return bool( this->x || this->y || this->z ); }
        friend bool operator	== ( const fvector& a, const fvector& b ) { return a.x == b.x && a.y == b.y && a.z == b.z; }
        friend bool operator	!= ( const fvector& a, const fvector& b ) { return !( a == b ); }

        double x, y, z;
    };

    //size: 0x10
    struct fvector2d
    {
        fvector2d( ) : x( 0 ), y( 0 ) {}
        fvector2d( double x, double y ) : x( x ), y( y ) {}

        fvector2d operator		+ ( const fvector2d& other ) const { return { this->x + other.x, this->y + other.y }; }
        fvector2d operator		- ( const fvector2d& other ) const { return { this->x - other.x, this->y - other.y }; }
        fvector2d operator		* ( double offset ) const { return { this->x * offset, this->y * offset }; }
        fvector2d operator		/ ( double offset ) const { return { this->x / offset, this->y / offset }; }

        fvector2d& operator		*= ( const double other ) { this->x *= other; this->y *= other; return *this; }
        fvector2d& operator		/= ( const double other ) { this->x /= other; this->y /= other; return *this; }

        fvector2d& operator		=  ( const fvector2d& other ) { this->x = other.x; this->y = other.y; return *this; }
        fvector2d& operator		+= ( const fvector2d& other ) { this->x += other.x; this->y += other.y; return *this; }
        fvector2d& operator		-= ( const fvector2d& other ) { this->x -= other.x; this->y -= other.y; return *this; }
        fvector2d& operator		*= ( const fvector2d& other ) { this->x *= other.x; this->y *= other.y; return *this; }
        fvector2d& operator		/= ( const fvector2d& other ) { this->x /= other.x; this->y /= other.y; return *this; }

        operator bool( ) { return bool( this->x || this->y ); }
        friend bool operator	==( const fvector2d& a, const fvector2d& b ) { return a.x == b.x && a.y == b.y; }
        friend bool operator	!=( const fvector2d& a, const fvector2d& b ) { return !( a == b ); }

        double x, y;
    };


    //size: 0x18
    struct frotator
    {
        frotator( ) : pitch( 0 ), yaw( 0 ), roll( 0 ) {}
        frotator( double pitch, double yaw, double roll ) : pitch( pitch ), yaw( yaw ), roll( roll ) {}

        frotator operator		+ ( const frotator& other ) const { return { this->pitch + other.pitch, this->yaw + other.yaw, this->roll + other.roll }; }
        frotator operator		- ( const frotator& other ) const { return { this->pitch - other.pitch, this->yaw - other.yaw, this->roll - other.roll }; }
        frotator operator		* ( double offset ) const { return { this->pitch * offset, this->yaw * offset, this->roll * offset }; }
        frotator operator		/ ( double offset ) const { return { this->pitch / offset, this->yaw / offset, this->roll / offset }; }

        frotator& operator		*= ( const double other ) { this->yaw *= other; this->pitch *= other; this->roll *= other; return *this; }
        frotator& operator		/= ( const double other ) { this->yaw /= other; this->pitch /= other; this->roll /= other; return *this; }

        frotator& operator		=  ( const frotator& other ) { this->pitch = other.pitch; this->yaw = other.yaw; this->roll = other.roll; return *this; }
        frotator& operator		+= ( const frotator& other ) { this->pitch += other.pitch; this->yaw += other.yaw; this->roll += other.roll; return *this; }
        frotator& operator		-= ( const frotator& other ) { this->pitch -= other.pitch; this->yaw -= other.yaw; this->roll -= other.roll; return *this; }
        frotator& operator		*= ( const frotator& other ) { this->pitch *= other.pitch; this->yaw *= other.yaw; this->roll *= other.roll; return *this; }
        frotator& operator		/= ( const frotator& other ) { this->pitch /= other.pitch; this->yaw /= other.yaw; this->roll /= other.roll; return *this; }

        operator bool( ) { return bool( this->pitch || this->yaw || this->roll ); }
        friend bool operator	== ( const frotator& a, const frotator& b ) { return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll; }
        friend bool operator	!= ( const frotator& a, const frotator& b ) { return !( a == b ); }

        double pitch, yaw, roll;
    };

    //size: 0x10
    struct flinearcolor
    {
        flinearcolor( ) : r( 0.f ), g( 0.f ), b( 0.f ), a( 0.f ) {}
        flinearcolor( float r, float g, float b, float a ) : r( r ), g( g ), b( b ), a( a ) {}

        operator bool( ) { return bool( this->r || this->g || this->b || this->a ); }
        friend bool operator	== ( const flinearcolor& a, const flinearcolor& b ) { return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a; }

        float r, g, b, a;
    };


    struct bounds_t {
        float left, right, top, bottom;
    };
}