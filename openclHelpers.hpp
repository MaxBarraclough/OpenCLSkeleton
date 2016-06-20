#if defined(_MSC_VER) && (_MSC_VER >= 1020)
  #pragma once
#endif

#ifndef OPENCL_HELPERS_HPP
  #define OPENCL_HELPERS_HPP 1

#include "main.hpp" // for DEBUG_MODE


// Note: if this sequence produces strange "Unexpected end of file" issues in VS2012,
// this due to a known VS bug regarding Unix endlines.
// https://connect.microsoft.com/VisualStudio/feedback/details/794991/c-error-directive-and-unix-line-endings-leads-to-an-unexpected-end-of-file
// This is the reason we use #pragma message immediately before #error
#ifdef INTEL_OPENCL_DEBUGGING
  #ifndef FORCE_USE_OF_CPU_DEVICE
    #pragma message (" Error: INTEL_OPENCL_DEBUGGING defined, but FORCE_USE_OF_CPU_DEVICE not ")
    #error INTEL_OPENCL_DEBUGGING defined, but FORCE_USE_OF_CPU_DEVICE not
  #endif

  #ifndef DEBUG_MODE
    #pragma message (" Error: INTEL_OPENCL_DEBUGGING defined, but DEBUG_MODE not ")
    #error INTEL_OPENCL_DEBUGGING defined, but DEBUG_MODE not
  #endif
#endif


#ifdef DEBUG_MODE
  #include <string>
#endif


namespace OpenCL_Helpers {
  void printAllDevices();
  void printAllPlatforms();

  SuccessValue searchAllPlatformsForBestDevice(cl_device_id &deviceRef, unsigned int &deviceVersionRef_out);
  void printDeviceInfo(cl_device_id device);

  void dumpBinaryOfProgram(cl_program program);

#ifdef DEBUG_MODE
  SuccessValue convertToString(const char *filename, std::string& s);
#endif
}

#endif // #ifndef OPENCL_HELPERS_HPP
