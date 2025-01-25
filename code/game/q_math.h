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

#ifndef Q_MATH_H
#define Q_MATH_H

/*
==============================================================

MATHLIB

==============================================================
*/

// angle indexes
#define	PITCH           0           // up / down
#define	YAW             1           // left / right
#define	ROLL            2           // fall over

#define	SIDE_FRONT		0
#define	SIDE_BACK		1
#define	SIDE_ON			2
#define	SIDE_CROSS		3

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;

#ifndef M_PI
#define M_PI		3.14159265358979323846			// matches value in gcc v2 math.h
#endif

#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( (a) * 180.0f ) / M_PI )

#define	EQUAL_EPSILON	0.001

// Color
#define	MAKERGB( v, r, g, b ) v[0]=r;v[1]=g;v[2]=b
#define	MAKERGBA( v, r, g, b, a ) v[0]=r;v[1]=g;v[2]=b;v[3]=a

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"

extern	vec4_t		colorBlack;
extern	vec4_t		colorRed;
extern	vec4_t		colorGreen;
extern	vec4_t		colorBlue;
extern	vec4_t		colorYellow;
extern	vec4_t		colorMagenta;
extern	vec4_t		colorCyan;
extern	vec4_t		colorWhite;
extern	vec4_t		colorLtGrey;
extern	vec4_t		colorMdGrey;
extern	vec4_t		colorDkGrey;

extern	vec4_t		g_color_table[8];

extern	vec3_t	vec3_origin;
extern	vec4_t	vec4_origin;
extern	vec3_t	axisDefault[3];

struct cplane_s;

#define	nanmask (255<<23)
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

#define Q_max( x, y )   ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#define Q_min( x, y )   ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define Q_sign( f )     ( ( f > 0 ) ? 1 : ( ( f < 0 ) ? -1 : 0 ) )

float Q_rsqrt(float f);		// reciprocal square root
float Q_fabs(float f);

#define Square(x) ((x)*(x))
#define SQRTFAST( x ) ( (x) * Q_rsqrt( x ) )

float ClampFloat(float min, float max, float value);
signed char ClampChar(int i);
signed short ClampShort(int i);

#if	1

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define CrossProduct(a,b,c)		((c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1],(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2],(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0])

#define DotProduct4(x,y)		((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2]+(x)[3]*(y)[3])
#define VectorSubtract4(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2],(c)[3]=(a)[3]-(b)[3])
#define VectorAdd4(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2],(c)[3]=(a)[3]+(b)[3])
#define VectorCopy4(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])
#define	VectorScale4(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s),(o)[3]=(v)[3]*(s))
#define	VectorMA4(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s),(o)[3]=(v)[3]+(b)[3]*(s))

#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])

#define	SnapVector(v)			{v[0]=(int)v[0];v[1]=(int)v[1];v[2]=(int)v[2];}

#else

#define DotProduct(x,y)			_DotProduct(x,y)
#define VectorSubtract(a,b,c)	_VectorSubtract(a,b,c)
#define VectorAdd(a,b,c)		_VectorAdd(a,b,c)
#define VectorCopy(a,b)			_VectorCopy(a,b)
#define	VectorScale(v, s, o)	_VectorScale(v,s,o)
#define	VectorMA(v, s, b, o)	_VectorMA(v,s,b,o)
#define CrossProduct(a,b,c)		_CrossProduct(a,b,c)

#define DotProduct4(x,y)		_DotProduct4(x,y)
#define VectorSubtract4(a,b,c)	_VectorSubtract4(a,b,c)
#define VectorAdd4(a,b,c)		_VectorAdd4(a,b,c)
#define VectorCopy4(a,b)		_VectorCopy4(a,b)
#define	VectorScale4(v, s, o)	_VectorScale4(v, s, o)
#define	VectorMA4(v, s, b, o)	_VectorMA4(v, s, b, o)

#define VectorClear(a)			_VectorClear(a)
#define VectorNegate(a,b)		_VectorNegate(a,b)
#define VectorSet(v, x, y, z)	_VectorSet(v, x, y, z)
#define Vector4Copy(a,b)		_Vector4Copy(a,b)

#define	SnapVector(v)			_SnapVector(v)

#endif

int		Q_log2(int val);
float	Q_acos(float c);

int		Q_rand(int* seed);
float	Q_random(int* seed);
float	Q_crandom(int* seed);

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

float	AngleMod(float a);
float	LerpAngle(float from, float to, float frac);
float	AngleSubtract(float a1, float a2);

float AngleNormalize360(float angle);
float AngleNormalize180(float angle);
float AngleDelta(float angle1, float angle2);

#ifdef __cplusplus			// so we can include this in C code

#include "math_vector.h"
#include "math_angles.h"
#include "math_matrix.h"
#include "math_quaternion.h"

class idVec3_t;						// for defining vectors
typedef idVec3_t& vec3_p;				// for passing vectors as function arguments
typedef const idVec3_t& vec3_c;		// for passing vectors as const function arguments

class angles_t;						// for defining angle vectors
typedef angles_t& angles_p;			// for passing angles as function arguments
typedef const angles_t& angles_c;	// for passing angles as const function arguments

class mat3_t;						// for defining matrices
typedef mat3_t& mat3_p;				// for passing matrices as function arguments
typedef const mat3_t& mat3_c;		// for passing matrices as const function arguments

// this isn't a real cheap function to call!
int DirToByte(const idVec3_t& dir);
void ByteToDir(int b, vec3_p dir);

float NormalizeColor(vec3_c in, vec3_p out);

int VectorCompare(vec3_c v1, vec3_c v2);
float VectorLength(vec3_c v);
float Distance(vec3_c p1, vec3_c p2);
float DistanceSquared(vec3_c p1, vec3_c p2);
float VectorNormalize(vec3_p v);		// returns vector length
void VectorNormalizeFast(vec3_p v);		// does NOT return vector length, uses rsqrt approximation
float VectorNormalize2(vec3_c v, vec3_p out);
void VectorInverse(vec3_p v);
void VectorRotate(vec3_c in, mat3_c matrix, vec3_p out);
void VectorPolar(vec3_p v, float radius, float theta, float phi);
void VectorSnap(vec3_p v);
void Vector53Copy(const idVec5_t& in, vec3_p out);
void Vector5Scale(const idVec5_t& v, float scale, idVec5_t& out);
void Vector5Add(const idVec5_t& va, const idVec5_t& vb, idVec5_t& out);
void VectorRotate3(vec3_c vIn, vec3_c vRotation, vec3_p out);
void VectorRotate3Origin(vec3_c vIn, vec3_c vRotation, vec3_c vOrigin, vec3_p out);

float Q_rint(float in);

void vectoangles(vec3_c value1, angles_p angles);
void AnglesToAxis(angles_c angles, mat3_p axis);

void AxisCopy(mat3_c in, mat3_p out);
bool AxisRotated(mat3_c in);			// assumes a non-degenerate axis

int SignbitsForNormal(vec3_c normal);
int BoxOnPlaneSide(const Bounds& b, struct cplane_s* p);

void AnglesSubtract(angles_c v1, angles_c v2, angles_p v3);

bool PlaneFromPoints(idVec4_t& plane, vec3_c a, vec3_c b, vec3_c c);
void ProjectPointOnPlane(vec3_p dst, vec3_c p, vec3_c normal);
void RotatePointAroundVector(vec3_p dst, vec3_c dir, vec3_c point, float degrees);
void RotateAroundDirection(mat3_p axis, float yaw);
void MakeNormalVectors(vec3_c forward, vec3_p right, vec3_p up);
// perpendicular vector could be replaced by this

int	PlaneTypeForNormal(vec3_c normal);

void MatrixMultiply(mat3_c in1, mat3_c in2, mat3_p out);
void MatrixInverseMultiply(mat3_c in1, mat3_c in2, mat3_p out);	// in2 is transposed during multiply
void MatrixTransformVector(vec3_c in, mat3_c matrix, vec3_p out);
void MatrixProjectVector(vec3_c in, mat3_c matrix, vec3_p out); // Places the vector into a new coordinate system.
void AngleVectors(angles_c angles, vec3_p forward, vec3_p right, vec3_p up);
void PerpendicularVector(vec3_p dst, vec3_c src);

float TriangleArea(vec3_c a, vec3_c b, vec3_c c);

#else

// this isn't a real cheap function to call!
int DirToByte(vec3_t dir);
void ByteToDir(int b, vec3_t dir);

unsigned ColorBytes3(float r, float g, float b);
unsigned ColorBytes4(float r, float g, float b, float a);

float NormalizeColor(const vec3_t in, vec3_t out);

float RadiusFromBounds(const vec3_t mins, const vec3_t maxs);
void ClearBounds(vec3_t mins, vec3_t maxs);
void AddPointToBounds(const vec3_t v, vec3_t mins, vec3_t maxs);

int VectorCompare(const vec3_t v1, const vec3_t v2);
vec_t VectorLength(const vec3_t v);
vec_t VectorLengthSquared(const vec3_t v);
vec_t Distance(const vec3_t p1, const vec3_t p2);
vec_t DistanceSquared(const vec3_t p1, const vec3_t p2);
void VectorNormalizeFast(vec3_t v);
void VectorInverse(vec3_t v);

vec_t VectorNormalize(vec3_t v);		// returns vector length
vec_t VectorNormalize2(const vec3_t v, vec3_t out);
void Vector4Scale(const vec4_t in, vec_t scale, vec4_t out);
void VectorRotate(vec3_t in, vec3_t matrix[3], vec3_t out);

void VectorToAngles(const vec3_t value1, vec3_t angles);
void AnglesToAxis(const vec3_t angles, vec3_t axis[3]);

void AxisClear(vec3_t axis[3]);
void AxisCopy(vec3_t in[3], vec3_t out[3]);

void SetPlaneSignbits(struct cplane_s* out);
int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, struct cplane_s* plane);

void AnglesSubtract(vec3_t v1, vec3_t v2, vec3_t v3);

bool PlaneFromPoints(vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c);
void ProjectPointOnPlane(vec3_t dst, const vec3_t p, const vec3_t normal);
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);
void RotateAroundDirection(vec3_t axis[3], float yaw);
void MakeNormalVectors(const vec3_t forward, vec3_t right, vec3_t up);
// perpendicular vector could be replaced by this

//int	PlaneTypeForNormal (vec3_t normal);

void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void PerpendicularVector(vec3_t dst, const vec3_t src);
#endif			// __cplusplus

#endif // #ifndef Q_MATH_H
