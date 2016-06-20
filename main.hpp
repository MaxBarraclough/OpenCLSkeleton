// This should be the first file included in every of our cpp and (other) hpp files

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
  #pragma once
  #pragma warning(disable : 4482) // do not warn on MyEnum::MyConstant (which is legal in C++11)
#endif

#ifndef MAIN_HPP
#define MAIN_HPP 1


#define ENABLE_OPENCL_PROFILING 1


// Handle a Boost 1.56 bug which gives a bad definition of BOOST_TYPEOF for ICC 1500

// http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
// http://www.boost.org/doc/libs/release/libs/predef/doc/html/predef/reference/boost_comp_compiler_macros.html
#if ( defined(__ICC) || defined(__INTEL_COMPILER) )
        #if ( !defined(__INTEL_COMPILER) || __INTEL_COMPILER < 1500 )
                #error Error: old, untested version of Intel C++ compiler
        #else
                // Prevent other Boost header file from pulling in <boost/typeof/typeof.hpp>
                // and giving a definition not compatible with Intel C++ Compiler 1500
                #include <boost/typeof/typeof.hpp>
                // doesn't work: #define BOOST_TYPEOF_TYPEOF_HPP_INCLUDED
                #undef BOOST_TYPEOF
                #define BOOST_TYPEOF decltype
        #endif
#else
        #include <boost/typeof/typeof.hpp>
#endif


#include "stdlib.h" // exit

void ourExit(int status);

// From Chromium via https://stackoverflow.com/a/4415646
#define COUNT_OF(x) (  (sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x]))))  )


/**
 * Intuitive DEBUG_MODE Definition For Windows
 *
 * Building on Unix, CMake has a lot of control over the build,
 * but on Windows or when using an IDE, the IDE decides if you're
 * really doing a debug build (CMake's BUILD_TYPE counts for much less).
 *
 * When switching to a debug build in Unix, the user will re-run
 * one of the shell-scripts, and CMake will be run for a Debug build.
 * On Windows, it's a drag-and-drop selection, which CMake is unaware of.
 *
 * DEBUG_MODE is defined as 1 by CMake in case of a CMake-level debug-build,
 * but will be left undefined if the user chooses to do a debug-build
 * at the IDE level (without re-running CMake).
 *
 * Here, we ensure DEBUG_MODE is defined in such a case.
 */
#ifdef DEBUG_MODE
  // Already defined, so do nothing
#else
  #if (defined DEBUG || defined _DEBUG)
    #define DEBUG_MODE 1
  #endif
#endif

// #undef DEBUG_MODE

// BOOST_STATIC_ASSERT((CL_VERSION_1_2 == 120));
// AMD fails this! The macro is defined as 1, not as 120,
// despite https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/preprocessorDirectives.html
// so WE MUST AVOID THOSE MACROS!
// They indicate acknowledgement of existence, not value definition

#define CL_VERSION_1_0_NUM 100
#define CL_VERSION_1_1_NUM 110
#define CL_VERSION_1_2_NUM 120
#define CL_VERSION_2_0_NUM 200
#define CL_VERSION_2_1_NUM 210

#define ENSURE_OPENCL_1_2 1



///// TODO move into a config.hpp file


// #define USE_OPENCL_2 1
//* // Comment out the whole block if you don't mind CPU or GPU
#if 1 // Set to 1:force GPU or 0:force CPU
  #define FORCE_USE_OF_GPU_DEVICE 1
  // #define OPENCL_1_1_COMPATIBILITY 1
#else // we'll want this for debugging on Intel
  #define FORCE_USE_OF_CPU_DEVICE 1
  // #define INTEL_OPENCL_DEBUGGING 1 // CPU only: Intel can't yet debug on GPU
#endif
/**/

#ifdef FORCE_USE_OF_GPU_DEVICE
  #pragma message (" WE ARE FORCING USE OF GPU DEVICE ")
#elif defined FORCE_USE_OF_CPU_DEVICE
  #pragma message (" WE ARE FORCING USE OF CPU DEVICE ")
#endif
/* */

#ifdef INTEL_OPENCL_DEBUGGING
  #ifndef DEBUG_MODE
    #pragma message (" Warning: INTEL_OPENCL_DEBUGGING is set, but not DEBUG_MODE ")
  #endif
#endif

#define STRINGIFY_WITH_EXPANSION(S) STRINGIFY_NO_EXPANSION(S)
#define STRINGIFY_NO_EXPANSION(S)   #S


#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

// included by both host C++ code *and* by the OpenCL C code. Useful for sharing constants.
#include "cl_macros.h"




enum SuccessValue {
#ifdef DEBUG_MODE
        DUMMY_VAL_1, DUMMY_VAL_2,
        // ensure that values are checked against
        // FAILURE_VAL and SUCCESS_VAL and not just treated as bool
#endif
    FAILURE_VAL, SUCCESS_VAL, UNINITIALISED_VAL
};

// const /*implicitly static*/ bool load_from_binary_gc = false;
const /*implicitly static*/ bool dump_binary_blob_gc = true; // !load_from_binary_gc;



const /*implicitly static*/ size_t inputBufferNumElems = 128;
const /*implicitly static*/ size_t inputBufferSize = inputBufferNumElems * sizeof(cl_int);


#endif // ifndef MAIN_HPP
