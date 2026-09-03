#ifndef __ANO_MATH_H
#define __ANO_MATH_H

#define REAL float
#define TAN_MAP_RES 0.003921569f /* (smallest non-zero value in table) */
#define RAD_PER_DEG 0.017453293f
#define DEG_PER_RAD 57.29577951f
#define TAN_MAP_SIZE 256
#define MY_PPPIII 3.14159f
#define MY_PPPIII_HALF 1.570796f

#define my_sign(x) (((x) > 1e-6f) ? 1 : (((x) < -1e-6f) ? -1 : 0))
#define my_3_norm(x, y, z) (my_sqrt(my_pow((x)) + my_pow((y)) + my_pow((z))))
#define my_2_norm(x, y) (my_sqrt(my_pow((x)) + my_pow((y))))

#define my_pow(a) ((a) * (a))
#define safe_div(numerator, denominator, safe_value) ((denominator == 0) ? (safe_value) : ((numerator) / (denominator)))
#define ABS(x) ((x) > 0 ? (x) : -(x))

//#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max)? (max) : (x) ) )
#define LIMIT(x, min, max) (((x) <= (min)) ? (min) : (((x) > (max)) ? (max) : (x)))
#define DELTA_LIMIT(x, dl, y) (y) += (LIMIT(((x) - (y)), -dl, dl))

#define _MIN(a, b) ((a) < (b) ? (a) : (b))
#define _MAX(a, b) ((a) > (b) ? (a) : (b))

#define my_pow_2_curve(in, a, max) (((1.0f - (a)) + (a)*LIMIT(ABS((in) / (max)), 0, 1)) * in)

//#define RECTANGLE2CIRCLE_FACTOR(x,y,l) (safe_div(_MAX(ABS((x)),ABS((y))),(l),0))
//#define RECTANGLE2CIRCLE_FACTOR(r,l) (safe_div((r),(l),0))

#define To_180_degrees range_to_180deg
#define range_to_180deg(a) ((a) > 180 ? (a - 360) : ((a) < -180 ? (a + 360) : (a)))

#endif