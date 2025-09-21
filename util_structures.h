#pragma once
#include "util_base.h"

//Kinda inspired by using zig for some time

//Allocator interface like zig

//Everything is 'aligned' allocations
//Since data is also a pointer, this struct can and better be passed by value
//For now realloc is not used as issues may occur with alignments
//Realloc behaviour is the same as other standard behavior
//It's an undefined behaviour to reallocate memory and change the alignment of a block.
//But just try to mitigate such behaviour
typedef struct Alloc_Interface Alloc_Interface;
struct Alloc_Interface {
  //For now, all align values should be a power of 2
  void* (*alloc)(void* p_data, size_t size, size_t align);
  //void* (*realloc)(void* p_data, void* pmem, size_t size, size_t align);
  void (*free)(void* p_data, void* pmem); //No nullptr checking is guarenteed by interface
  //p_data, if needed, is assumed to be valid, so using invalid p_data is UB
  void* p_data;
};

//Aligned malloc supported interface
//Needs stdlib, need to support some realloc features
#include <stdlib.h>
#include <string.h>// for memset/memcpy

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <malloc.h>
#endif
UTILAPI void* aligned_malloc_wrapper_(void* p_data, size_t size, size_t align){
  (void)p_data;
  //Need to use separate thing for unix or posix windows, and separate for non posix windows
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
  return _aligned_malloc(size, align);
  //return malloc(size);
#else
  return aligned_alloc(align, size);
#endif
}
UTILAPI void aligned_mfree_wrapper_(void* p_data, void* pmem){
  (void)p_data;
  //free(pmem);
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
  _aligned_free(pmem);
  //free(pmem);
#else
  free(pmem);
#endif
}

UTILAPI Alloc_Interface gen_std_allocator(void){
  return (Alloc_Interface){
    .alloc = aligned_malloc_wrapper_,
    .free = aligned_mfree_wrapper_
  };
}

UTILAPI void* alloc_mem(Alloc_Interface allocr, size_t size, size_t align){
  void* res = allocr.alloc(allocr.p_data, size, align);
  //printf("Allocating %zu bytes at %p\n", size, res);
  return res;
}

UTILAPI void free_mem(Alloc_Interface allocr, void* pmem){
  //printf("Freeing %p\n", pmem);
  allocr.free(allocr.p_data, pmem);
}


//ALways use a 'typedefed' data type for this purpose
//A slice struct making macro
//Gives a
//slice iff ptr is not null

#define DEF_SLICE_STR(base_type)						\
  typedef struct base_type##_Slice base_type##_Slice;			\
  struct base_type##_Slice {						\
    base_type* data;							\
    size_t count;							\
  };

#define DEF_SLICE_FXN(base_type)						\
  UTILAPI base_type##_Slice init_##base_type##_slice(base_type* ptr, size_t count){ \
    if(ptr == nullptr) return (base_type##_Slice){0};			\
    return(base_type##_Slice){.data = ptr, .count = count};		\
  }									\
  UTILAPI size_t base_type##_slice_bytes(base_type##_Slice slice){	\
    return sizeof(base_type) * slice.count;				\
  }									\
  UTILAPI base_type##_Slice						\
  make_copy_##base_type##_slice(Alloc_Interface allocr,			\
				base_type##_Slice src){			\
    base_type##_Slice new_slice =					\
      SLICE_ALLOC(allocr, base_type, src.count);			\
    if(new_slice.data == nullptr) return new_slice;			\
    memcpy(new_slice.data, src.data, base_type##_slice_bytes(src));	\
    return new_slice;							\
  }


#define DEF_SLICE(base_type)						\
  DEF_SLICE_STR(base_type);						\
  DEF_SLICE_FXN(base_type);

#define SLICE_FROM_ARRAY(base_type, array)		\
  init_##base_type##_slice(array, _countof(array))

#define MAKE_ARRAY_SLICE(base_type, ...)	\
  SLICE_FROM_ARRAY(base_type, ((base_type[]){__VA_ARGS__}))

#define SLICE_REINTERPRET(src_type, dst_type, src)			\
  init_##dst_type##_slice((dst_type*)(src).data,			\
			  ((src).count * sizeof(src_type))/sizeof(dst_type))

//Slice malloc, accepts any malloc style function
#define SLICE_MALLOC(malloc_func, base_type, count)		\
  init_##base_type##_slice((base_type*)(malloc_func)(count * sizeof(base_type)), count)
//Frees only if not nullptr, follows stdlib's free
#define SLICE_MFREE(free_func, slice)				\
  do{if((slice).data != nullptr) { (free_func)(slice.data); }}while(0)


//Slice alloc, accepts a allocator interface struct, sets aligmnet by alignof
#define SLICE_ALLOC(alloc_interface, base_type, count)	\
  init_##base_type##_slice((base_type*)alloc_mem((alloc_interface), ((count) * sizeof(base_type)), alignof(base_type)), (count))
//Frees only if not nullptr, uses allocator interface struct, sets to zero
#define SLICE_FREE(alloc_interface, slice)	\
  do{						\
    if((slice).data != nullptr) {		\
      free_mem((alloc_interface), (slice).data);	\
    }						\
    (slice).data = nullptr;			\
    (slice).count = 0;				\
  }while(0)

// A get/set kind of thing for slice that also asserts for data pointer not being null
//and the index is >=0 and < specified length 
#define slice_inx(slice, inx) (*((assert(((void)"Trying to access slice where it is absent", ((slice).data != nullptr))), assert(((void)"Slice index out of range", ( (inx) < (slice).count && (inx)>= 0  )))), &((slice).data[inx])))

#define for_slice(slice, inx) for_range(size_t, inx, 0, (slice).count)

//A pair of slices of same length, just used for wrapping
//A macro to combine two slices and generate a slice pair object, such that the count is of the smaller slice
#define DEF_SLICE_PAIR(type1, type2)					\
  typedef struct type1##type2##_Slice type1##type2##_Slice;	\
  struct type1##type2##_Slice {				\
    type1* data1;						\
    type2* data2;						\
    size_t count;						\
  };								\
  UTILAPI type1##type2##_Slice type1##type2##_slices_to_pair	\
  (type1##_Slice a, type2##_Slice b){			\
    if((a.data == nullptr) || (b.data == nullptr)		\
       || (a.count == 0) || (b.count == 0))			\
      return (type1##type2##_Slice){0};			\
    return (type1##type2##_Slice){				\
      .data1 = a.data,						\
      .data2 = b.data,						\
      .count = (a.count < b.count)?a.count:b.count};		\
  }								\

#define DEF_SLICE_TRIPLE(type1, type2, type3)				\
  typedef struct type1##type2##type3##_Slice type1##type2##type3##_Slice;	\
  struct type1##type2##type3##_Slice {					\
    type1* data1;						\
    type2* data2;						\
    type3* data3;						\
    size_t count;						\
  };								\
  UTILAPI type1##type2##type3##_Slice type1##type2##type3##_slices_to_triple	\
  (type1##_Slice a, type2##_Slice b, type3##_Slice c){					\
    if((a.data == nullptr) || (b.data == nullptr) ||(c.data == nullptr)	\
       || (a.count == 0) || (b.count == 0) || (c.count == 0))		\
      return (type1##type2##type3##_Slice){0};				\
    return (type1##type2##type3##Slice){				\
      .data1 = a.data,						\
      .data2 = b.data,						\
      .data3 = c.data,						\
      .count = (a.count < b.count)?a.count:b.count};		\
  }								\


//A dynamic array implementation

//Now implement a simple dynamic array, that has a minimum limit on allocation,
//Unless 0 number of elements, when it frees all
//Allocates 2x when not enough, frees extra allocated when 1/4th is only used
//Will take in an allocator at the start

UTILAPI bool adjust_darray(Alloc_Interface allocr, void** baseptr,
		      size_t *psize, size_t* pcap, size_t align,
		      size_t lower_limit, sptr change){
  //Does not validate null pointers and such
  //Does no change if invalid request

  if((change < 0) && (*psize < (size_t)(-change)))
    return false;

  const size_t new_size = (sptr)*psize + change;

  //If increasing, checks against capacity
  //If decreasing, checks against 0, lower limit or capacity/4
  if(change >= 0){
    if(new_size <= *pcap){
      *psize = new_size;
      return true;
    }
  }
  else{
    if((new_size != 0) &&
       (new_size >= *pcap/4) &&
       (new_size >= lower_limit)){
      *psize = new_size;
      return true;
    }
  }
  void* newptr = nullptr;
  size_t new_cap = *pcap;
  if(new_size != 0){
    if(change > 0){
      if(new_cap < lower_limit)
	new_cap = lower_limit;
      while(new_cap < new_size)
	new_cap *= 2;
    }
    else{
      while(true){
	if(((new_cap / 2) < lower_limit) ||
	   (new_cap / 2) < 2 * new_size)
	  break;
	new_cap = new_cap / 2;
      }
    }
    newptr = alloc_mem(allocr, new_cap , align);
    if(newptr == nullptr){
      if(change < 0){
	//Handle popping specially so it never fails due to allocation error
	new_cap = *pcap;
	newptr = *baseptr;
      }
      else
	return false;
    }

    memcpy(newptr, *baseptr, _min(*psize, new_size));
  }
  else{
    new_cap = 0;
  }

  if(nullptr != *baseptr)
    free_mem(allocr, *baseptr);
  *pcap = new_cap;
  *baseptr = newptr;
  *psize = new_size;

  return true;
}

#define DEF_DARRAY(basetype, min_count)					\
  typedef struct basetype##_Darray basetype##_Darray;			\
  struct basetype##_Darray {						\
    Alloc_Interface allocr;						\
    basetype* data;							\
    size_t count;							\
    size_t capacity;							\
  };									\
  UTILAPI basetype##_Darray init_##basetype##_darray(Alloc_Interface allocr){ \
    return (basetype##_Darray){.allocr = allocr};			\
  }									\
  UTILAPI bool resize_##basetype##_darray(basetype##_Darray* arr, size_t new_count){ \
    void* ptr = arr->data;						\
    size_t size = arr->count * sizeof(basetype);			\
    size_t cap = arr->capacity * sizeof(basetype);			\
    const sptr adj = (sptr)sizeof(basetype)*new_count - (sptr)size;	\
    bool res = adjust_darray(arr->allocr, &ptr, &size, &cap,		\
				alignof(basetype),			\
				min_count * sizeof(basetype), adj);	\
    if(res){								\
      arr->data = ptr;							\
      arr->count = size / sizeof(basetype);				\
      arr->capacity = cap / sizeof(basetype);				\
    }									\
    return res;								\
  }									\
  UTILAPI bool push_##basetype##_darray(basetype##_Darray* arr, basetype val){ \
    bool res = resize_##basetype##_darray(arr, arr->count + 1);	\
    if(res){								\
      arr->data[arr->count -1] = val;					\
    }									\
    return res;								\
  }									\
  UTILAPI bool pop_##basetype##_darray(basetype##_Darray* arr, size_t count){ \
    if(count > arr->count) return false;				\
    bool res = resize_##basetype##_darray(arr, arr->count - count);	\
    return res;								\
  }									\
  UTILAPI bool downsize_##basetype##_darray(basetype##_Darray* arr, size_t pos, size_t amt){ \
    if(pos > arr->count) return false;					\
    size_t src_off = pos + amt;						\
    if(src_off > arr->count) return false;				\
    if(src_off < arr->count)						\
      memmove((void*)(arr->data + pos), (void*)(arr->data + src_off),	\
	      (arr->count-src_off) * sizeof(basetype));			\
    return pop_##basetype##_darray(arr, amt);				\
  }	       
