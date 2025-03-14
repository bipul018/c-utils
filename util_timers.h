#pragma once
#include <time.h>
#include "util_units.h"

#define SECONDS 1ULL
#define MINUTES 60ULL
#define HOURS   (MINUTES * 60ULL)
#define DAYS    (HOURS * 24ULL)
#define WEEKS   (DAYS * 7ULL)
#define YEARS   (DAYS * 365ULL)
typedef struct timespec timespec;

//Now only windows is supported here, uses windows.h directly
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

UTILAPI timespec get_timer_delta(struct timespec t1, struct timespec t0){

    t0.tv_sec = t1.tv_sec - t0.tv_sec;
    t0.tv_nsec = t1.tv_nsec - t0.tv_nsec;
    if(t0.tv_nsec < 0){
        t0.tv_nsec += ONE INANO;
        t0.tv_sec -= 1;
    }
    return t0;
}

UTILAPI timespec add_two_times(timespec t1, timespec t2){
    timespec res={0};
  uint64_t nsec = t1.tv_nsec;
  nsec += t2.tv_nsec;
  res.tv_sec = 0;
  if(nsec >= (ONE INANO)){
    res.tv_sec = 1;
    nsec = nsec - ONE INANO;
  }
  res.tv_nsec = nsec;
  res.tv_sec += t1.tv_sec + t2.tv_sec;
  return res;
}

UTILAPI timespec filetime_to_timespec(FILETIME ft){
  timespec ts;
  uint64_t times = ft.dwLowDateTime;
  times += (uint64_t)ft.dwHighDateTime << 32ULL;
  //1 unit in filetime = 100 ns
  ts.tv_nsec = times % ((ONE INANO)/100);
    //Convert back into nsec
  ts.tv_nsec *= 100;
  ts.tv_sec = times / ((ONE INANO)/100);

  return ts;
}

UTILAPI timespec start_process_timer()
{
  FILETIME create, exited, kernel, user;
  if(!GetProcessTimes(GetCurrentProcess(),&create,&exited,&kernel,&user)){
    return (timespec){0};
  }

  return add_two_times(filetime_to_timespec(kernel), filetime_to_timespec(user));
}

UTILAPI timespec end_process_timer(timespec* time)
{
    struct timespec hold = *time;
    *time = start_process_timer();
    return get_timer_delta(*time, hold);
}

UTILAPI timespec start_thread_timer()
{
  FILETIME create, exited, kernel, user;
  if(!GetThreadTimes(GetCurrentThread(),&create,&exited,&kernel,&user)){
    return (timespec){0};
  }

  return add_two_times(filetime_to_timespec(kernel), filetime_to_timespec(user));
}

UTILAPI timespec end_thread_timer(timespec* time)
{
  struct timespec hold = *time;
  *time = start_thread_timer();
  return get_timer_delta(*time,hold);
}

UTILAPI LARGE_INTEGER timer_perf_freq = {0};
UTILAPI timespec start_monotonic_timer()
{
  if(!timer_perf_freq.QuadPart){
    QueryPerformanceFrequency(&timer_perf_freq);
  }
  LARGE_INTEGER ticks;
  QueryPerformanceCounter(&ticks);
  //Assume 64 bit support
  timespec ts;
  ts.tv_sec = ticks.QuadPart / timer_perf_freq.QuadPart;
  ts.tv_nsec = (ticks.QuadPart - ts.tv_sec * timer_perf_freq.QuadPart) *( ONE INANO / timer_perf_freq.QuadPart);
  return ts;
}

UTILAPI timespec end_monotonic_timer(timespec* time)
{
  struct timespec hold = *time;
  *time = start_monotonic_timer();
  return get_timer_delta(*time,hold);
}

UTILAPI double timer_sec(timespec del){
    return (double)del.tv_sec + (double)del.tv_nsec UNANO;
}

UTILAPI double timer_milli(timespec del){
    return (double)del.tv_sec IMILLI + (double)del.tv_nsec UNIT_UP(NANO,MILLI);
}

UTILAPI double timer_micro(timespec del){
    return (double)del.tv_sec IMICRO + (double)del.tv_nsec UNIT_UP(NANO,MICRO);
}

UTILAPI double timer_nano(timespec del){
    return (double)del.tv_sec INANO + (double)del.tv_nsec;
}

UTILAPI double timer_mins(timespec del){
    return ((double)del.tv_sec  + (double)del.tv_nsec UNANO )UNIT_UP(SECONDS,MINUTES);
}

UTILAPI double timer_hrs(timespec del){
    return ((double)del.tv_sec  + (double)del.tv_nsec UNANO )UNIT_UP(SECONDS,HOURS);
}


UTILAPI double get_moving_avg(double *arr, int* inx, int size, double newval){
    assert(arr && inx);
    *inx = _clamp(*inx,0,size-1);
    arr[*inx] = newval;
    double sum = 0.0;
    for_range(int,i,0,size){
        sum += arr[i];
    }
    sum /= size;

    (*inx) = (*inx + 1) % size;
    return sum;
}


UTILAPI void get_moving_avg_faster(double* arr, int* inx, int size, double newval, double* out_val){
    assert(arr && inx && out_val);
    *inx = _clamp(*inx,0,size-1);
    (*out_val) = (newval - arr[*inx]) / size;
    arr[*inx] = newval;
}

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
