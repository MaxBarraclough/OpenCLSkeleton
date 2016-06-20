#include "main.hpp" // will pull in OpenCL for us, among other things

#include "errorHandling.hpp"

#include <iostream>
#include <algorithm> // equal_range

#include "projAsserts.hpp" // include this one last


#define ERROR_HANDLING_OUTPUT_STREAM std::cerr

#ifdef ENABLE_RUNTIME_ASSERTS

void boost::assertion_failed(char const * expr, char const * function, char const * file, long line) {
        ERROR_HANDLING_OUTPUT_STREAM << "\nError executing (" << expr << ") in function " << function << " in file " << file << ", line " << line << std::endl;

        ERROR_HANDLING_OUTPUT_STREAM.flush();

        ourExit(1);
}

void boost::assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line) {
        ERROR_HANDLING_OUTPUT_STREAM << "\nError executing (" << expr << ") in function " << function << " in file " << file << ", line " << line << '\n';
        ERROR_HANDLING_OUTPUT_STREAM << "Reason: " << msg << std::endl;

        ERROR_HANDLING_OUTPUT_STREAM.flush();

        ourExit(1);
}
#endif



inline static bool comp(const I_S_Pair &l, const I_S_Pair &r) {
        return l.first < r.first;
}

void printError(const char * const funcStr, const I_S_Pair * begin, const I_S_Pair * end, cl_int val) {

        ERROR_HANDLING_OUTPUT_STREAM << "\nError in OpenCL function: ";
        ERROR_HANDLING_OUTPUT_STREAM << funcStr;
        ERROR_HANDLING_OUTPUT_STREAM << "\nError code: ";

        const auto p = std::equal_range( begin, end, std::make_pair(val,(char*)NULL), comp );

        if (  p.first->first == val  ) {
                ERROR_HANDLING_OUTPUT_STREAM << p.first->second;
                ERROR_HANDLING_OUTPUT_STREAM << '\n';
        } else {
                ERROR_HANDLING_OUTPUT_STREAM << "<<Invalid error code>>\n";
        }
}


// clEnqueueNDRangeKernel
const I_S_Pair endrkArr[16] = {
        std::make_pair( CL_INVALID_GLOBAL_WORK_SIZE, "CL_INVALID_GLOBAL_WORK_SIZE" ),
        std::make_pair( CL_INVALID_EVENT_WAIT_LIST, "CL_INVALID_EVENT_WAIT_LIST" ),
        std::make_pair( CL_INVALID_GLOBAL_OFFSET, "CL_INVALID_GLOBAL_OFFSET" ),
        std::make_pair( CL_INVALID_WORK_ITEM_SIZE, "CL_INVALID_WORK_ITEM_SIZE" ),
        std::make_pair( CL_INVALID_WORK_GROUP_SIZE, "CL_INVALID_WORK_GROUP_SIZE" ),
        std::make_pair( CL_INVALID_WORK_DIMENSION, "CL_INVALID_WORK_DIMENSION" ),
        std::make_pair( CL_INVALID_KERNEL_ARGS, "CL_INVALID_KERNEL_ARGS" ),
        std::make_pair( CL_INVALID_KERNEL, "CL_INVALID_KERNEL" ),
        std::make_pair( CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE" ),
        std::make_pair( CL_INVALID_IMAGE_SIZE, "CL_INVALID_IMAGE_SIZE" ),
        std::make_pair( CL_INVALID_COMMAND_QUEUE, "CL_INVALID_COMMAND_QUEUE" ),
        std::make_pair( CL_INVALID_CONTEXT, "CL_INVALID_CONTEXT" ),
        std::make_pair( CL_MISALIGNED_SUB_BUFFER_OFFSET, "CL_MISALIGNED_SUB_BUFFER_OFFSET" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" ),
        std::make_pair( CL_MEM_OBJECT_ALLOCATION_FAILURE, "CL_MEM_OBJECT_ALLOCATION_FAILURE" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(endrkArr) == 16 ));


// clBuildProgram
const I_S_Pair bpemArr[10] = {
        std::make_pair( CL_INVALID_OPERATION, "CL_INVALID_OPERATION" ),
        std::make_pair( CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM" ),
        std::make_pair( CL_INVALID_BUILD_OPTIONS, "CL_INVALID_BUILD_OPTIONS" ),
        std::make_pair( CL_INVALID_BINARY, "CL_INVALID_BINARY" ),
        std::make_pair( CL_INVALID_DEVICE, "CL_INVALID_DEVICE" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_BUILD_PROGRAM_FAILURE, "CL_BUILD_PROGRAM_FAILURE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" ),
        std::make_pair( CL_COMPILER_NOT_AVAILABLE, "CL_COMPILER_NOT_AVAILABLE" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(bpemArr) == 10 ));


// clCreateBuffer
const I_S_Pair cbemArr[7] = {
        std::make_pair( CL_INVALID_BUFFER_SIZE, "CL_INVALID_BUFFER_SIZE" ),
        std::make_pair( CL_INVALID_HOST_PTR, "CL_INVALID_HOST_PTR" ),
        std::make_pair( CL_INVALID_CONTEXT, "CL_INVALID_CONTEXT" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" ),
        std::make_pair( CL_MEM_OBJECT_ALLOCATION_FAILURE, "CL_MEM_OBJECT_ALLOCATION_FAILURE" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(cbemArr) == 7 ));


// clCreateKernel
const I_S_Pair ckemArr[7] = {
        std::make_pair( CL_INVALID_KERNEL_DEFINITION, "CL_INVALID_KERNEL_DEFINITION" ),
        std::make_pair( CL_INVALID_KERNEL_NAME, "CL_INVALID_KERNEL_NAME" ),
        std::make_pair( CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE" ),
        std::make_pair( CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(ckemArr) == 7 ));



// Assumption: exactly one binary was attempted to be loaded.
// Only if this is true is it appropriate to accept a single binaryStatus value rather than an array
void printClCreateProgramWithBinary(const cl_int status, const cl_int binaryStatus) {
#ifdef ENABLE_RUNTIME_ASSERTS
        BOOST_ASSERT_MSG((status != CL_SUCCESS), "No error to print: CL_SUCCESS is not an error-code");
#endif

        const char * toPrint = "Invalid error status";

        switch(status) {
        case CL_INVALID_BINARY:
                toPrint = "CL_INVALID_BINARY";
                break;
        case CL_INVALID_CONTEXT:
                toPrint = "CL_INVALID_CONTEXT";
                break;
        case CL_INVALID_DEVICE:
                toPrint = "CL_INVALID_DEVICE";
                break;
        case CL_INVALID_VALUE:
                toPrint = "CL_INVALID_VALUE";
                break;
        case CL_OUT_OF_HOST_MEMORY:
                toPrint = "CL_OUT_OF_HOST_MEMORY";
                break;
        case CL_OUT_OF_RESOURCES:
                toPrint = "CL_OUT_OF_RESOURCES";
                break;
        }

        ERROR_HANDLING_OUTPUT_STREAM << "Error in OpenCL function: clCreateProgramWithBinary\nError code: ";
        ERROR_HANDLING_OUTPUT_STREAM << toPrint;
        ERROR_HANDLING_OUTPUT_STREAM << "\n and 'binary status' error-code: ";

        toPrint = "Invalid error status"; // reset

        switch(binaryStatus) {
        case CL_SUCCESS:
                toPrint = "CL_SUCCESS";
                break;

        case CL_INVALID_VALUE:
                toPrint = "CL_INVALID_VALUE";
                break;

        case CL_INVALID_BINARY:
                toPrint = "CL_INVALID_BINARY";
                break;
        }

        ERROR_HANDLING_OUTPUT_STREAM << toPrint;
        ERROR_HANDLING_OUTPUT_STREAM << std::endl;
}


// clGetProgramBuildInfo
const I_S_Pair pbiArr[5] = {
        std::make_pair( CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM" ),
        std::make_pair( CL_INVALID_DEVICE, "CL_INVALID_DEVICE" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(pbiArr) == 5 ));


// clSetKernelArg
const I_S_Pair skaArr[9] = {
        std::make_pair( CL_INVALID_DEVICE_QUEUE, "CL_INVALID_DEVICE_QUEUE" ),
        std::make_pair( CL_INVALID_ARG_SIZE, "CL_INVALID_ARG_SIZE" ),
        std::make_pair( CL_INVALID_ARG_VALUE, "CL_INVALID_ARG_VALUE" ),
        std::make_pair( CL_INVALID_ARG_INDEX, "CL_INVALID_ARG_INDEX" ),
        std::make_pair( CL_INVALID_KERNEL, "CL_INVALID_KERNEL" ),
        std::make_pair( CL_INVALID_SAMPLER, "CL_INVALID_SAMPLER" ),
        std::make_pair( CL_INVALID_MEM_OBJECT, "CL_INVALID_MEM_OBJECT" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(skaArr) == 9 ));


// clGetPlatformIDs
#ifndef CL_PLATFORM_NOT_FOUND_KHR
  #define CL_PLATFORM_NOT_FOUND_KHR (-1001) // https://www.khronos.org/registry/cl/extensions/khr/cl_khr_icd.txt
#endif
const I_S_Pair gpiArr[3] = {
        std::make_pair( CL_PLATFORM_NOT_FOUND_KHR, "CL_PLATFORM_NOT_FOUND_KHR" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" )
};
BOOST_STATIC_ASSERT(( COUNT_OF(gpiArr) == 3 ));



// clCreateContext
#ifndef CL_INVALID_DX9_MEDIA_ADAPTER_KHR
  #define CL_INVALID_DX9_MEDIA_ADAPTER_KHR (-1010)
#endif

#ifndef CL_INVALID_D3D11_DEVICE_KHR
  #define CL_INVALID_D3D11_DEVICE_KHR (-1006)
#endif

#ifndef CL_INVALID_D3D10_DEVICE_KHR
  #define CL_INVALID_D3D10_DEVICE_KHR (-1002)
#endif

#ifndef CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR
  #define CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR (-1000)
#endif

const I_S_Pair ccArr[12] = {
        std::make_pair( CL_INVALID_DX9_MEDIA_ADAPTER_KHR, "CL_INVALID_DX9_MEDIA_ADAPTER_KHR" ),
        std::make_pair( CL_INVALID_D3D11_DEVICE_KHR, "CL_INVALID_D3D11_DEVICE_KHR" ),
        std::make_pair( CL_INVALID_D3D10_DEVICE_KHR, "CL_INVALID_D3D10_DEVICE_KHR" ),
        std::make_pair( CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR, "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR" ),
        std::make_pair( CL_INVALID_PROPERTY, "CL_INVALID_PROPERTY" ),
        std::make_pair( CL_INVALID_OPERATION, "CL_INVALID_OPERATION" ),
        std::make_pair( CL_INVALID_DEVICE, "CL_INVALID_DEVICE" ),
        std::make_pair( CL_INVALID_PLATFORM, "CL_INVALID_PLATFORM" ),
        std::make_pair( CL_INVALID_VALUE, "CL_INVALID_VALUE" ),
        std::make_pair( CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY" ),
        std::make_pair( CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES" ),
        std::make_pair( CL_DEVICE_NOT_AVAILABLE, "CL_DEVICE_NOT_AVAILABLE" ),
        // std::make_pair( CL_INVALID_ADAPTER_KHR, "CL_INVALID_ADAPTER_KHR" ), // macro doesn't seem to exist
};
BOOST_STATIC_ASSERT(( COUNT_OF(ccArr) == 12 ));


// clGetProgramInfo
const I_S_Pair prgInfArr[5] = {
    std::make_pair(CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE"),
    std::make_pair(CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM"),
    std::make_pair(CL_INVALID_VALUE, "CL_INVALID_VALUE"),
    std::make_pair(CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY"),
    std::make_pair(CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES")
};
BOOST_STATIC_ASSERT((COUNT_OF(prgInfArr) == 5));



void printOpenCLBuildLog(cl_program program, cl_device_id device) {
        size_t sz = 0;
        cl_int status = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &sz);

        CHECK_CLGETPROGRAMBUILDINFO(status);

        BOOST_ASSERT(( 0 != sz ));
        void * const allocated = malloc(sz);
        BOOST_ASSERT(( NULL != allocated ));

        status = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sz, allocated, NULL);
        BOOST_ASSERT(( CL_SUCCESS == status ));

        ERROR_HANDLING_OUTPUT_STREAM << (char*)allocated;
        free(allocated);
}

/** Ensure array is sorted and without repetition (strictly ascending) */
 static BOOST_CXX14_CONSTEXPR SuccessValue test_I_S_Pair_array(const I_S_Pair * const arr, const size_t count) {
        SuccessValue ret = SuccessValue::SUCCESS_VAL;
        if (count > 1) {

          cl_int lowBound = arr[0].first;

          for (size_t i = 1; i != count; ++i) {
                  if ( arr[i].first > lowBound ) { // it's ok
                        lowBound = arr[i].first;
                  } else { // bad array
                    ret = SuccessValue::FAILURE_VAL;
                    break;
                  }
          }
        }
        return ret;
}

 static BOOST_CXX14_CONSTEXPR SuccessValue testAllArrays() {
        const bool ok =
                   ( test_I_S_Pair_array( ckemArr,  COUNT_OF(ckemArr)  ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( endrkArr, COUNT_OF(endrkArr) ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( bpemArr,  COUNT_OF(bpemArr)  ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( cbemArr,  COUNT_OF(cbemArr)  ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( pbiArr,   COUNT_OF(pbiArr)   ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( skaArr,   COUNT_OF(skaArr)   ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( gpiArr,   COUNT_OF(gpiArr)   ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( ccArr,    COUNT_OF(ccArr)    ) == SuccessValue::SUCCESS_VAL )
                && ( test_I_S_Pair_array( prgInfArr,COUNT_OF(prgInfArr)) == SuccessValue::SUCCESS_VAL )
                ;
        return (ok ? SuccessValue::SUCCESS_VAL : SuccessValue::FAILURE_VAL);
}

#ifdef BOOST_NO_CXX14_CONSTEXPR // can't test array sortedness at compile time
  #ifdef DEBUG_MODE
    // safe: construction order guaranteed within a compilation object
    static struct S {
      S() {
        BOOST_ASSERT_MSG( (testAllArrays() == SuccessValue::SUCCESS_VAL), "Array-sortedness checks failed" );
      }
    } dummy;
 #endif
#else
  BOOST_STATIC_ASSERT(( testAllArrays() == SuccessValue::SUCCESS_VAL ));
  // may be necessary to pass a compiler flag to allow this
#endif
