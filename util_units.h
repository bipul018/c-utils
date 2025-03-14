#pragma once
#include "util_base.h"

#include <stdbool.h>
#include <inttypes.h>
#include <float.h>

#if defined(INT8_C)
typedef int8_t s8;
#endif
#if defined(INT16_C)
typedef int16_t s16;
#endif
#if defined(INT32_C)
typedef int32_t s32;
#endif
#if defined(INT64_C)
typedef int64_t s64;
#endif
#if defined(INTPTR_MAX)
typedef intptr_t sptr;
#endif
#if defined(INTMAX_C)
typedef intmax_t sbig;
#endif
#if defined(INT_FAST32_MAX)
typedef int_fast32_t sf32;
#endif

#if defined(UINT8_C)
typedef uint8_t u8;
#endif
#if defined(UINT16_C)
typedef uint16_t u16;
#endif
#if defined(UINT32_C)
typedef uint32_t u32;
#endif
#if defined(UINT64_C)
typedef uint64_t u64;
#endif
#if defined(UINTPTR_MAX)
typedef uintptr_t uptr;
#endif
#if defined(UINTMAX_C)
typedef uintmax_t ubig;
#endif
#if defined(UINT_FAST32_MAX)
typedef uint_fast32_t uf32;
#endif

static_assert(sizeof(float) == 4, "weird platform : float not of 4 bytes aren't supported");
static_assert(sizeof(double) == 8, "weird platform : double not of 4 bytes aren't supported");
typedef float f32;
typedef double f64;


//Unit conversion macros , for 1000 multiples

//Converts these to normal units
#define NANO  1ULL
#define MICRO 1000ULL
#define MILLI 1000000ULL
#define BASE  1000000000ULL
#define KILO  1000000000000ULL
#define MEGA  1000000000000000ULL
#define GIGA  1000000000000000000ULL

#define _unit_factor(from, to) (((to) > (from))?((to)/(from)):((from)/(to)))
#define UNIT_UP(from, to) / ((to)/(from))
#define UNIT_DOWN(from, to) * ((from)/(to))

//Converts these units to normal units {units}

#define UNANO  / (BASE / NANO )
#define UMICRO / (BASE / MICRO)
#define UMILLI / (BASE / MILLI)
#define UKILO  * (BASE / KILO )
#define UMEGA  * (BASE / MEGA )
#define UGIGA  * (BASE / GIGA )

//Converts normal units to these {inverse units}

#define INANO  * (BASE / NANO )
#define IMICRO * (BASE / MICRO)
#define IMILLI * (BASE / MILLI)
#define IKILO  / (BASE / KILO )
#define IMEGA  / (BASE / MEGA )
#define IGIGA  / (BASE / GIGA )

#define ONE 1ULL

#define SIZE_KB(size) (((unsigned long long)size) << 10ULL)
#define SIZE_MB(size) (((unsigned long long)size) << 20ULL)
#define SIZE_GB(size) (((unsigned long long)size) << 30ULL)


