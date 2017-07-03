#pragma once

#ifndef PI
#	define PI	(3.1415926535897932384626433832795)
#endif

#ifndef Ln2
#	define Ln2	(0.69314718055994530941723212145818)
#endif

#ifndef CONST_E
#	define CONST_E	(2.71828182845904523536)
#endif

#ifndef MIN
#	define MIN(a, b)	((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#	define MAX(a, b)	((a) < (b) ? (b) : (a))
#endif

#ifndef RADIAN 
#	define RADIAN(angle)	(((float)(angle)/180.0) * PI)
#endif

#ifndef ANGLE
#	define ANGLE(radian)	(((float)(radian)/PI) * 180)
#endif


#ifndef FLOOR
#	define FLOOR(value, floor) ( (value)<(floor) ? (floor) : (value))
#endif

#ifndef CEIL
#	define CEIL(value, ceil)   ( (value)>(ceil) ? (ceil) : (value))
#endif




// -- from OpenCV --
/* min & max without jumps */
#define JZ_IMIN(a, b)  ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))
#define JZ_IMAX(a, b)  ((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

/* absolute value without jumps */
#ifndef __cplusplus
#  define	JZ_IABS(a)     (((a) ^ ((a) < 0 ? -1 : 0)) - ((a) < 0 ? -1 : 0))
#else
#  define	JZ_IABS(a)     abs(a)
#endif

#define JZ_FABS(a)		((a) >= 0 ? (a) : (-1* (a)))

#define JZ_CMP(a,b)    (((a) > (b)) - ((a) < (b)))
#define JZ_SIGN(a)     JZ_CMP((a),0)

