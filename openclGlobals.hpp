#if defined(_MSC_VER) && (_MSC_VER >= 1020)
  #pragma once
#endif

#ifndef OPENCL_GLOBALS_HPP
#define OPENCL_GLOBALS_HPP 1

#include "main.hpp"



extern cl_command_queue commandQueue_g;

extern cl_context context_g;

extern cl_device_id device_g;

extern unsigned int device_version_g;

extern cl_program main_program_g;

extern int bufferData[8];
extern cl_mem input_buffer_g;

extern cl_kernel main_kernel_g;
extern cl_kernel sortAndCopy_kernel_g;
extern cl_ulong device_localMemSz_g;


void initialiseTopLevelOpenCLGlobals();

void releaseTopLevelOpenCLGlobals();

void releaseBuffers();

void initialiseBuffers();

void initialiseKernels();

void releaseKernels();


#endif // #ifndef OPENCL_GLOBALS_HPP
