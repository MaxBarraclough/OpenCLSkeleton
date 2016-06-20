#if 1

#define CL_VERSION_1_0_NUM 100
#define CL_VERSION_1_1_NUM 110
#define CL_VERSION_1_2_NUM 120
#define CL_VERSION_2_0_NUM 200
#define CL_VERSION_2_1_NUM 210

inline static void sanityCheck() {
  // https://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/preprocessorDirectives.html
  __OPENCL_VERSION__;
  __OPENCL_C_VERSION__;
  __FILE__;
  __LINE__;
  // CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE;  // in OpenCL 2.0 and above

// The AMD OpenCL engine is buggy: the CL_VERSION_* macros are missing
//  CL_VERSION_1_0;
//  CL_VERSION_1_1;
//  CL_VERSION_1_2;
//  CL_VERSION_2_0;

  // test whether a standard OpenCL function can be called
  float4 v = (float4)(4.0f, 5.0f, 6.0f, 7.0f);
  dot(v,v); // should work for an arg of type float or float2 or float3 or float4
}



#if ( __OPENCL_VERSION__  >= CL_VERSION_1_2_NUM )
//#if ( __OPENCL_VERSION__ >= CL_VERSION_2_0_NUM )
  // ok
#else // version is too low

  #if ( __OPENCL_VERSION__ == CL_VERSION_1_0_NUM )
    #error Too old: OpenCL 1.0

  #elif ( __OPENCL_VERSION__ == CL_VERSION_1_1_NUM )
    #error Too old: OpenCL 1.1

//  #elif ( __OPENCL_VERSION__ == CL_VERSION_1_2_NUM )
//    #error Too old: OpenCL 1.2

  #else
    #error Unexpected value of __OPENCL_VERSION__
  #endif
#endif



#if ( __OPENCL_C_VERSION__  >= CL_VERSION_1_2_NUM )
//#if ( __OPENCL_C_VERSION__ >= CL_VERSION_2_0_NUM )
  // ok
#else // version is too low

  #if ( __OPENCL_C_VERSION__ == CL_VERSION_1_0_NUM )
    #error Too old: OpenCL 1.0

  #elif ( __OPENCL_C_VERSION__ == CL_VERSION_1_1_NUM )
    #error Too old: OpenCL 1.1

//  #elif ( __OPENCL_C_VERSION__ == CL_VERSION_1_2_NUM )
//    #error Too old: OpenCL 1.2

  #else
    #error Strange value of __OPENCL_C_VERSION__
  #endif
#endif



#ifndef __IMAGE_SUPPORT__
  #error Unexpected lack of image support on this OpenCL device
  #error ( __IMAGE_SUPPORT__ is undefined )
#endif

/*__kernel  __attribute__((reqd_work_group_size(1, 1, 1))) //*/ inline static
  void anotherTestForOcl(
    // __read_write // OpenCL 2.0 and above
  //read_write // OpenCL 2.0 and above
  __read_only
  image2d_t img) { }



__kernel  __attribute__((reqd_work_group_size(1, 1, 1)))
  void test(__global int* buf, int a, int b, int c) {
    int z = a * b;
        z += c;
        *buf = c;
  }

__kernel  __attribute__((reqd_work_group_size(1, 1, 1)))
  void test2(__global int* buf, int a, int b, int c) {
    int z = a + b;
        z *= c;
        *buf = c;
  }



  #endif





const __constant char * const __constant printMe = "Hello from OpenCL\n"; // AMD's OpenCL likes printf to take a __constant string
// note the __constant which should not be ommitted (though some platforms allows this)

__kernel  __attribute__((reqd_work_group_size(1, 1, 1)))
  void doStuff(__global int * buf) {
    //// printf(printMe);

        int j = 7;
        int k = 8;
        int m = 9;

        buf[0] = 3;
        buf[1] = 4;
        buf[2] = 5;
}


