#ifndef CL_MACROS_H
#define CL_MACROS_H 1


// Fail at host-code compile-time if compiled against old OpenCL (pre 1.2)
#ifdef __cplusplus
  #ifndef CL_MEM_HOST_NO_ACCESS
    #error CL_MEM_HOST_NO_ACCESS macro is not defined
  #endif
#endif


// Fail at online-compilation-time if run with old OpenCL
#ifdef __OPENCL_C_VERSION__ // if OpenCL C
  #if ( __OPENCL_C_VERSION__ >= 120 )
    // ok
  #else
    #error Too old version of OpenCL detected. We require 1.2 or newer.
  #endif
#endif


#define NUM_WGS 1

// #define NUM_WGS 65536

#define WG_SIZE 1



//#ifdef __OPENCL_C_VERSION__
//#endif


#define GLOBAL_WORK_SZ (NUM_WGS * WG_SIZE)

#if (GLOBAL_WORK_SZ % WG_SIZE != 0) // even-divisibility invariant
  #error Error: Total number of work-items must be divisible by work-group size
#endif


#endif // #ifndef CL_MACROS_H
