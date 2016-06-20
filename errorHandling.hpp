#if defined(_MSC_VER) && (_MSC_VER >= 1020)
  #pragma once
#endif

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP 1

#include "main.hpp"

#include <iostream>
#include <utility>   // make_pair
#include <iterator>  // std::begin std::end

#include "projAsserts.hpp" // include this last


void printClCreateProgramWithBinary(cl_int status, cl_int binaryStatus);
void printOpenCLBuildLog(cl_program program, cl_device_id device);


typedef std::pair<cl_int, const char*> I_S_Pair;

void printError(const char * const funcStr, const I_S_Pair * begin, const I_S_Pair * end, cl_int val);


extern const I_S_Pair endrkArr[16];
extern const I_S_Pair bpemArr[10];
extern const I_S_Pair cbemArr[7];
extern const I_S_Pair ckemArr[7];
extern const I_S_Pair pbiArr[5];
extern const I_S_Pair skaArr[9];
extern const I_S_Pair gpiArr[3];
extern const I_S_Pair ccArr[12];
extern const I_S_Pair prgInfArr[5];


// TODO adjust more to the _NAMED format

#define CHECK_CLENQUEUENDRANGEKERNEL(STATUS)   { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clEnqueueNDRangeKernel", std::begin(endrkArr), std::end(endrkArr), (STATUS) ); ourExit(2); } }

#define CHECK_CLBUILDPROGRAM(STATUS)           { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clBuildProgram",         std::begin(bpemArr),  std::end(bpemArr),  (STATUS) ); ourExit(2); } }

#define CHECK_CLCREATEBUFFER(STATUS)           { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clCreateBuffer",         std::begin(cbemArr),  std::end(cbemArr),  (STATUS) ); ourExit(2); } }

#define CHECK_CLCREATEBUFFER_NAMED(STATUS,STR) { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clCreateBuffer: " STR,   std::begin(cbemArr),  std::end(cbemArr),  (STATUS) ); ourExit(2); } }

#define CHECK_CLCREATEKERNEL(STATUS)           { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clCreateKernel",         std::begin(ckemArr),  std::end(ckemArr),  (STATUS) ); ourExit(2); } }

#define CHECK_CLGETPROGRAMBUILDINFO(STATUS)    { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clGetProgramBuildInfo",  std::begin(pbiArr),   std::end(pbiArr),   (STATUS) ); ourExit(2); } }

#define CHECK_CLSETKERNELARG(STATUS)           { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clSetKernelArg",         std::begin(skaArr),   std::end(skaArr),   (STATUS) ); ourExit(2); } }

#define CHECK_CLGETPLATFORMIDS(STATUS)         { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clGetPlatformIDs",       std::begin(gpiArr),   std::end(gpiArr),   (STATUS) ); ourExit(2); } }

#define CHECK_CLCREATECONTEXT(STATUS)          { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clCreateContext",        std::begin(ccArr),    std::end(ccArr),    (STATUS) ); ourExit(2); } }

#define CHECK_CLGETPROGRAMINFO(STATUS)         { if ( CL_SUCCESS != (STATUS) ) \
{ printError( "clGetProgramInfo",       std::begin(prgInfArr),std::end(prgInfArr),(STATUS) ); ourExit(2); } }


// TODO handle clGetEventProfilingInfo clFinish clCreateCommandQueue[...] clReleaseMemObject clReleaseEvent
// clReleaseContext clReleaseCommandQueue clReleaseProgram clCreateProgramWithSource

// TODO define all macros as empty if ENABLE_RUNTIME_ASSERTS is defined


#endif // #ifndef ERROR_HANDLING_HPP
