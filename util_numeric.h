#pragma once
#include <float.h>
#include <limits.h>

//Don't use alignment values not power of 2
#define _align_up(size, alignment)                                   \
    ((((unsigned long long)size) + ((unsigned long long)alignment) - \
      1ULL) &                                                        \
     ~(((unsigned long long)alignment) - 1ULL))

//Don't use alignment not power of 2
#define _align_down(size, alignment)                                 \
    (~((unsigned long long)(alignment)-1ULL) &                       \
     ((unsigned long long)size))

//Gives 0, -1 or 1 as sign, beware for float types of precision errors
#define _signof_zero(x, zero) (((x)>zero)?1:(((x) < zero)?-1:0))

//Automatic accounting for FLT_/DBL epsilons
#define _signof(x)				\
  _Generic((x),					\
	   float: (float)_signof_zero(x,FLT_EPSILON),	\
	   default: _Generic((x),					\
			     double: (double)_signof_zero(x, DBL_EPSILON), \
			     default: _Generic((x),			\
					       long double:		\
					       (long double)		\
					       _signof_zero(x,LDBL_EPSILON), \
					       default: _signof_zero(x, 0))))

//Dangerous to use with ++x;--x
#define _min(a,b) (((a) < (b))?(a):(b))
//Dangerous to use with ++x;--x
#define _max(a,b) (((a) > (b))?(a):(b))
//Do not use ++x;--x
#define _clamp(v, min, max) _max(min, _min(v, max))
//Donot pass literals, pass variables
//Maybe later replace that array hack with proper static assert
#define _swap(a, b)							\
  do{									\
    uint8_t error_swapping_arguments_of_different_sizes[(sizeof(a) == sizeof(b))?(ptrdiff_t)sizeof(a):-1];	\
    void* addr1 = &(a);\
    void* addr2 = &(b);\
    memcpy(error_swapping_arguments_of_different_sizes,addr1,sizeof(a));\
    memcpy(addr1,addr2,sizeof(a));\
    memcpy(addr2,error_swapping_arguments_of_different_sizes,sizeof(a));\
  }while(0)


// Some handy comparision functions that work by converting values first to (u)intptr type
#define _IS0(expr) ((uptr)(expr) == 0)
#define _ISNT0(expr) ((uptr)(expr) != 0)
#define _ISPOS(value) ((sptr)(value) > 0)
#define _ISNEG(value) ((sptr)(value) < 0)


