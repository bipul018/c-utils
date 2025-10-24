#pragma once
#include <assert.h>

//Concat and stringify macros
#define CONCAT_INTERNAL_(A, B) A##B
#define CONCAT(A,B) CONCAT_INTERNAL_(A,B)

#define STRINGIFY_INTERNAL_(A) #A
#define STRINGIFY(A) STRINGIFY_INTERNAL_(A)

#define nullptr ((void*)0)
typedef void(voidfn)(void);
#define nullfnptr ((voidfn*)0)

#define member_size(type, member) sizeof(((type *)0)->member)
// TODO:: Decide if you want to be C23 only
#define member_type(type, member) typeof(((type *)0)->member)

//If condition meets, it jumps to label
/* #define jmp_on_true(condition, label) \ */
/*   do{				      \ */
/*     if(condition){goto label;}	      \ */
/*   }				      \ */
/*   while(0) */

//If condition meets, it evaluates rest of the expression and jumps to label
#define jmp_on_true_expr(condition, label, ...)	\
  do{						\
    if(condition){				\
      __VA_ARGS__;				\
      goto label;				\
    }						\
  }						\
  while(0)


#include <stdalign.h>
//Fucking MSVC, not working with normal alignof
#ifndef alignof
#define alignof _Alignof
#endif

//Countof macro, is not typesafe, i.e, won't detect array
//Need probably typeof(), which is not available pre c23 :(
#ifndef _countof
#define _countof(arr)(uptr)(sizeof(arr)/sizeof(arr[0]))
#endif

//Maybe don't use these nargs things ??, maybe the comma thing won't work for c11 
#if defined(__GNUC__) || defined(__clang__)
// Supports 0-15 arguments
#define MAX_VA_NARGS_COUNT 15
#define VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N
// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
#define VA_NARGS(...) VA_NARGS_IMPL(_, ## __VA_ARGS__,15,14,13,12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
#define MAX_VA_NARGS_COUNT 15
// Supports 1-15 arguments
#define VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,_11, _12, _13, _14, _15, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(, ## __VA_ARGS__,15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif


// Make a FOREACH macro
// From : https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros/11994395#11994395
// Slightly modified to also get the number of argument as argument
// An implementation of for each for variadic macros to use
#define FE_0(WHAT)
#define FE_1(N, WHAT, X) WHAT((N),X) 
#define FE_2(N, WHAT, X, ...) WHAT((N),X)FE_1((N+1),WHAT, __VA_ARGS__)
#define FE_3(N, WHAT, X, ...) WHAT((N),X)FE_2((N+1),WHAT, __VA_ARGS__)
#define FE_4(N, WHAT, X, ...) WHAT((N),X)FE_3((N+1),WHAT, __VA_ARGS__)
#define FE_5(N, WHAT, X, ...) WHAT((N),X)FE_4((N+1),WHAT, __VA_ARGS__)
#define FE_6(N, WHAT, X, ...) WHAT((N),X)FE_5((N+1),WHAT, __VA_ARGS__)
#define FE_7(N, WHAT, X, ...) WHAT((N),X)FE_6((N+1),WHAT, __VA_ARGS__)
#define FE_8(N, WHAT, X, ...) WHAT((N),X)FE_7((N+1),WHAT, __VA_ARGS__)
#define FE_9(N, WHAT, X, ...) WHAT((N),X)FE_8((N+1),WHAT, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME
#define FOR_EACH_VA(action,...)						\
  GET_MACRO(_0,__VA_ARGS__,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0)(0, action,__VA_ARGS__)


// A for each macro, but passes 'N' in reverse (last element gets 0)
// But the upside from the `FOR_EACH_VA` is that the counter is a preprocessor constant
//     and addition of more entries is easier (just copy paste the definition)
#define FOR_EACH_VA2(action, ...)		\
  CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(action, __VA_ARGS__)

#define FE2_0(...) 
#define FE2_1(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
#define FE2_2(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
#define FE2_3(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
#define FE2_4(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
#define FE2_5(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
#define FE2_6(what, x, ...) what(VA_NARGS(__VA_ARGS__), x) CONCAT(FE2_, VA_NARGS(__VA_ARGS__))(what, __VA_ARGS__)
// ... repeat as needed, only the definition identifier has to be changed

// A macro built by stackoverflow codes + chatgpt adjustments 
#define _IS_ZERO_0 ~, 1
#define _CHECK_IS_ZERO(x)     _CHECK_IS_ZERO_HELPER(_IS_ZERO_##x)
#define _CHECK_IS_ZERO_HELPER(...)  _ARG_SECOND(__VA_ARGS__, 0)
#define _ARG_SECOND(_0, _1, ...)   _1
#define IF_0_1(TRUE_MACRO, FALSE_MACRO)  TRUE_MACRO
#define IF_0_0(TRUE_MACRO, FALSE_MACRO) FALSE_MACRO
#define IF_ZERO(x, TRUE_MACRO, FALSE_MACRO) \
    CONCAT(IF_0_, _CHECK_IS_ZERO(x))(TRUE_MACRO, FALSE_MACRO)

// Usage:
//#define MY_MACRO(a, ...)				\
//    IF_ZERO(a, expand_0, expand_other)(__VA_ARGS__)
// MY_MACRO(0, abc) => `expand_0(abc)`
// MY_MACRO(1, abc) => `expand_other(abc)`
// MY_MACRO(z, abc) => `expand_other(abc)`



//Helper macro to pass array directly like arr, count of arr
#define PASS_ARR_COUNT(array) (array),_countof(array)
//Helper macro to pass array directly like count of arr, arr
#define PASS_COUNT_ARR(array) _countof(array),(array)

//Maybe make for loop based, run once, defer like macros??

//Exclusive b , inclusive a
#define for_range(type, inx,a,b) \
for(type inx = ((1)?(a):(b)); (((a)<=(b))?(inx<(b)):(inx>(b)))  ; (((a) <= (b))?(inx+=1):(inx-=1)))

//Considers all data pointers are of same size
#define jpt_indirect_deref(ptr, offset)	\
  *((void**)( ((s8*)(ptr)) + (offset)  ))

