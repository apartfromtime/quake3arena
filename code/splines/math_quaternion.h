/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#ifndef __MATH_QUATERNION_H__
#define __MATH_QUATERNION_H__

#include <assert.h>
#include <math.h>

class idVec3_t;
class angles_t;
class mat3_t;

class quat_t {
public:
	float			x;
	float			y;
	float			z;
	float			w;

					quat_t();
					quat_t( float x, float y, float z, float w );

	friend void		toQuat( idVec3_t &src, quat_t &dst );
	friend void		toQuat( angles_t &src, quat_t &dst );
	friend void		toQuat( mat3_t &src, quat_t &dst );

	float			*vec4( void );
			
	float			operator[]( int index ) const;
	float			&operator[]( int index );

	void 			set( float x, float y, float z, float w );

	void			operator=( quat_t a );

	friend quat_t	operator+( quat_t a, quat_t b );
	quat_t			&operator+=( quat_t a );

	friend quat_t	operator-( quat_t a, quat_t b );
	quat_t			&operator-=( quat_t a );

	friend quat_t	operator*( quat_t a, float b );
	friend quat_t	operator*( float a, quat_t b );
	quat_t			&operator*=( float a );

	friend int		operator==(	quat_t a, quat_t b );
	friend int		operator!=(	quat_t a, quat_t b );

	float			Length( void );
	quat_t			&Normalize( void );

	quat_t			operator-();
};

Q_INLINE quat_t::quat_t() {
}

Q_INLINE quat_t::quat_t( float x, float y, float z, float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Q_INLINE float *quat_t::vec4( void ) {
	return &x;
}

Q_INLINE float quat_t::operator[]( int index ) const {
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

Q_INLINE float& quat_t::operator[]( int index ) {
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

Q_INLINE void quat_t::set( float x, float y, float z, float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Q_INLINE void quat_t::operator=( quat_t a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;
}

Q_INLINE quat_t operator+( quat_t a, quat_t b ) {
	return quat_t( a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w );
}

Q_INLINE quat_t& quat_t::operator+=( quat_t a ) {
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
}

Q_INLINE quat_t operator-( quat_t a, quat_t b ) {
	return quat_t( a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w );
}

Q_INLINE quat_t& quat_t::operator-=( quat_t a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;

	return *this;
}

Q_INLINE quat_t operator*( quat_t a, float b ) {
	return quat_t( a.x * b, a.y * b, a.z * b, a.w * b );
}

Q_INLINE quat_t operator*( float a, quat_t b ) {
	return b * a;
}

Q_INLINE quat_t& quat_t::operator*=( float a ) {
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
}

Q_INLINE int operator==( quat_t a, quat_t b ) {
	return ( ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w ) );
}

Q_INLINE int operator!=( quat_t a, quat_t b ) {
	return ( ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z ) && ( a.w != b.w ) );
}

Q_INLINE float quat_t::Length( void ) {
	float length;
	
	length = x * x + y * y + z * z + w * w;
	return ( float )sqrt( length );
}

Q_INLINE quat_t& quat_t::Normalize( void ) {
	float length;
	float ilength;

	length = this->Length();
	if ( length ) {
		ilength = 1 / length;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
		
	return *this;
}

Q_INLINE quat_t quat_t::operator-() {
	return quat_t( -x, -y, -z, -w );
}

#endif /* !__MATH_QUATERNION_H__ */
