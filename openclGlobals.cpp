//#define DEBUG_MODE 1

//#include "main.hpp"
#include "openclGlobals.hpp" // includes "main.hpp" and OpenCL for us

#include "openclHelpers.hpp"
#include "errorHandling.hpp"

#include <boost/scope_exit.hpp>

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "projAsserts.hpp" // include this one last


cl_context context_g;
cl_device_id device_g;
unsigned int device_version_g;
cl_command_queue commandQueue_g;

cl_program main_program_g;


int bufferData[8];
cl_mem input_buffer_g;

cl_kernel main_kernel_g;
cl_kernel sortAndCopy_kernel_g;

cl_ulong device_localMemSz_g;

#ifdef DEBUG_MODE
  // Don't need any such array
#else
    static const char Include_char_arr[] =
      {'#', 'i', 'n', 'c', 'l', 'u', 'd', 'e', ' ', '"', 'm', 'a', 'i', 'n', '.', 'c', 'l', '"'};
#endif



static void wgSizeCheck() {
        size_t wgSz = 0;

        // gives "maximum work-group size that can be used to execute a kernel on a specific device"
        cl_int status = clGetKernelWorkGroupInfo(main_kernel_g, device_g, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &wgSz, NULL);

        BOOST_ASSERT_MSG( (CL_SUCCESS == status), "Error from clGetKernelWorkGroupInfo/CL_KERNEL_WORK_GROUP_SIZE" );
                // wgSz now holds maximum allowable work-group size, *not* the *actual* work-group size for any specific kernel
                if ( WG_SIZE <= wgSz ) { /* ok, so do nothing */ } else  { /* we've exceeded the limit */
                        std::cerr << "WG_SIZE is too great: ";
                        std::cerr << WG_SIZE;
                        std::cerr << " where maximum allowable value is: ";
                        std::cerr << wgSz;
                        std::cerr << std::endl;
                        ourExit(4);
                }
}


/**
 *  Create kernel objects.
 *  Depends upon the program objects having been initialised properly already.
 */
static void initialise_mainKernel() {

        cl_int status = !CL_SUCCESS;

        main_kernel_g = clCreateKernel( main_program_g, "doStuff", &status );
        CHECK_CLCREATEKERNEL(status);

        cl_uint argCounter = 0;

        status = clSetKernelArg( main_kernel_g, argCounter++, sizeof(cl_mem), (void*)&input_buffer_g );

        CHECK_CLSETKERNELARG( status );
}


void initialiseKernels() {
                initialise_mainKernel();
}

void releaseKernels() {
        cl_int status = clReleaseKernel(main_kernel_g);
        BOOST_ASSERT_MSG( (CL_SUCCESS == status), "Error releasing main_kernel_g");
}




void releaseBuffers() {
        cl_int status = clReleaseMemObject(input_buffer_g);
        BOOST_ASSERT_MSG( (CL_SUCCESS == status), "Error releasing input_buffer_g" );
        input_buffer_g = NULL;
}


// TODO remove needless parameters
void initialiseBuffers() {
        cl_int status = !CL_SUCCESS;

        cl_mem_flags inputBufferFlags =
            // CL_MEM_COPY_HOST_PTR     // Copy from host array into device memory
            CL_MEM_USE_HOST_PTR

            //| CL_MEM_READ_ONLY     // (On-device) kernels will read and *will NOT* write
            | CL_MEM_READ_WRITE;     // kernel both reads from, and writes to, the buffer

        // OPENCL_1_1_COMPATIBILITY // if using a version of OpenCL 1.2 or newer

        if (device_version_g >= CL_VERSION_1_2_NUM) {
            inputBufferFlags |= CL_MEM_HOST_NO_ACCESS; // do not use for OpenCL 1.1 and 1.0
            // Compatible with CL_MEM_COPY_HOST_PTR (affects only copy and map operations)
        }

        input_buffer_g = clCreateBuffer(
            context_g,
            inputBufferFlags,
            inputBufferSize,         // sizeof the whole array
            (void*)&bufferData,
            &status);

        CHECK_CLCREATEBUFFER_NAMED(status, "input_buffer_g");
}



/**
 * Creates program from source, and builds it
 */

#ifdef DEBUG_MODE
                static const char argsStr_basisArr[] =
                        //////////////////// NON-AMD PLATFORMS WILL NOT LIKE THESE:    "-fno-bin-source -fno-bin-llvmir -fno-bin-amdil -fbin-exe"  " " // http://devgurus.amd.com/message/1297425#1297425

//"-fno-bin-amdil -fbin-exe " // from http://marc.info/?l=john-dev&m=136962303231151&w=2


#if 0
                        "-fbin-exe"                                                                   " "
                        "-save-temps"                                                                 " "
                        "-fbin-llvmir"                                                             " "
                        "-fbin-amdil"                                                                 " " // http://openwall.info/wiki/john/development/AMD-IL
                        "-fbin-exe"                                                                   " "
#endif

#ifdef USE_OPENCL_2
                                                "–cl-std=CL2.0"                                                              " "
#endif

                        // "-fno-bin-llvmir -fno-bin-source -fno-bin-exe -fbin-amdil"                   " " // http://openwall.info/wiki/john/development/AMD-IL
                        "-D DEBUG_MODE=1"                                                            " "
                        //"-D MX_ELEMS_MC=" STRINGIFY_WITH_EXPANSION(MAX_ELEMS_PER_WI__LITERAL)        " "
                        //"-D WGSZ_MC=" STRINGIFY_WITH_EXPANSION(WG_SIZE)                              " "
                        //"-D NMWGS_MC=" STRINGIFY_WITH_EXPANSION(NUM_WGS)                               " "
                        "-I ."                                                                       " "
                        ;// "-cl-denorms-are-zero"                                                       " ";
                // see http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clBuildProgram.html
#else
                //////// TODO RIGOROUSLY PASS THE -g FLAG
                static const char argsStr[] =
#ifdef USE_OPENCL_2
                                                "–cl-std=CL2.0"                                                              " "
#endif
                        "-D MX_ELEMS_MC=" STRINGIFY_WITH_EXPANSION(MAX_ELEMS_PER_WI__LITERAL)        " "
                        "-D WGSZ_MC=" STRINGIFY_WITH_EXPANSION(WG_SIZE)                              " "
                        //"-D NMWGS_MC=" STRINGIFY_WITH_EXPANSION(NUM_WGS)                               " "
                        "-I . "
                        ;
                        //-g -s \"C:\\Users\\mb\\Documents\\main\\main.cl\"  "; /* -g is Intel's debug flag*/
//                        "-I kernels_root"                                                                     " "
//                        // "-cl-opt-disable"                                                                  " "
//                        "-cl-denorms-are-zero"                                                                " "
#endif




#ifdef DEBUG_MODE
static void initialiseGivenProgram_DEBUG(const char *filename, cl_program &programRef) {
        cl_int status = !CL_SUCCESS;
        {
                std::string sourceStr;
                const SuccessValue convStatus = OpenCL_Helpers::convertToString(filename, sourceStr);

                BOOST_ASSERT_MSG( (SuccessValue::SUCCESS_VAL == convStatus),
                        "Unable to read file into string. Are you running this application from the correct directory?" );

                const char *source = sourceStr.c_str(); // may not itself be const: we take addr of this ptr later
                size_t sourceSize[] = { sourceStr.length() };
                // trivia: length() is guaranteed constant-time in C++11, i.e. faster than strlen

                programRef = clCreateProgramWithSource(context_g, 1, &source, sourceSize, &status); // returns cl_program, not status
                BOOST_ASSERT_MSG( (CL_SUCCESS == status), "clCreateProgramWithSource returned an error-code" );
        }

        {
#ifndef INTEL_OPENCL_DEBUGGING
  #define argsStr argsStr_basisArr // NOTE: in this case argsStr will be char[],
                                   // but using the code below, it will be char*
#else                              // so do not do anything like (sizeof argsStr)
                // We must 1: allocate space for new string
                //         2: copy over argsStr_basisArr
                //         3: append argsStr_firstPartOfSuffix
                //         4: append filename
                //         5: append prologue of "\""

#pragma message (" WARNING: assuming that we are based in C:\\Users\\mb\\Documents\\main ")
                std::cout << ("WARNING: assuming that we are based in C:\\Users\\mb\\Documents\\main" "\n");

                const char argsStr_firstPartOfSuffix[] = "-g -s \"C:/Users/mb/Documents/main/";
                // absence of a trailing space is vital

                // Step 1
                const size_t filename_strlen = strlen(filename);
                char * const argsStr = (BOOST_TYPEOF(argsStr)) alloca(sizeof argsStr_basisArr + sizeof argsStr_firstPartOfSuffix + filename_strlen);
                // we don't want to count the terminating chars from both literals,
                // but we *do* want space for a closing '"', so they balance out: no need to +1 or -1

                // Step 2
                memcpy(argsStr, argsStr_basisArr, sizeof argsStr_basisArr - 1); // -1 because we do not copy terminating char

                // Step 3
                char * copyToHere = argsStr + sizeof argsStr_basisArr - 1; // progress by number of chars we just copied
                memcpy(copyToHere, argsStr_firstPartOfSuffix, sizeof argsStr_firstPartOfSuffix - 1);

                // Step 4
                copyToHere += sizeof argsStr_firstPartOfSuffix - 1; // progress by number of chars we just copied
                memcpy(copyToHere, filename, filename_strlen);


                // Step 5
                copyToHere += filename_strlen;
                *copyToHere = '"';
                ++copyToHere;
                *copyToHere = '\0';

                // printf("<<%s>>\n",argsStr);
#endif

                std::cout << "---\n";
                std::cout << argsStr;
                std::cout << "\n---\n";

                status = clBuildProgram(programRef, 1 /*1 device only*/, &device_g, argsStr,
                        /*blocking (no callback)*/ NULL, NULL);

                CHECK_CLBUILDPROGRAM(status);
        }
}
#endif // #ifdef DEBUG_MODE


#ifdef DEBUG_MODE
static SuccessValue initialiseGivenProgram_DEBUG__FROM_BINARY(const char *filename, cl_program &programRef) {
                cl_int status = !CL_SUCCESS;
                {
                        unsigned char * buffer = NULL;
                        size_t fileLength = 0;
                        BOOST_SCOPE_EXIT( (&buffer) ) {
                                BOOST_ASSERT((NULL != buffer));
                                free(buffer);
                        } BOOST_SCOPE_EXIT_END

                                // based on http://www.cplusplus.com/reference/istream/istream/read/
                                typedef std::basic_ifstream<unsigned char> uifstream; // like ifstream, but for unsigned char not char
                                uifstream is (filename, uifstream::binary);
                                if (is) {
                                        // get length of file:
                                        is.seekg(0, is.end);
                                        fileLength = (size_t)is.tellg();
                                        is.seekg(0, is.beg);

                                        buffer = (BOOST_TYPEOF(buffer))malloc(fileLength);
                                        BOOST_ASSERT((NULL != buffer));

                                        std::cout << "Reading " << fileLength << " characters of binary file... ";
                                        // read data as a block:
                                        is.read(buffer,fileLength);

                                        if (is) {
                                            std::cout << "all characters read successfully\n";
                                        } else {
                                            std::cerr << "error: only " << is.gcount() << " could be read\n";
                                            BOOST_ASSERT_MSG(false, "Unable to read from binary file");
                                        }
                                        is.close();
                                } else {
                                        BOOST_ASSERT_MSG(false, "Error reading binary file");
                                }
                        BOOST_ASSERT_MSG( ( 0 != fileLength ), "Unexpected zero file-length" );

                        cl_int binaryStatus = !CL_SUCCESS;

                        // yes, we cast from  char*  to  const unsigned char*
                        // which is awful and dangerous, but ifstream insists on char

                        programRef = clCreateProgramWithBinary(context_g,
                                1, &device_g,
                                &fileLength,
                                (const unsigned char **)(&buffer), // cast to add constness http://stackoverflow.com/a/5055789
                                &binaryStatus, &status);

                        if ((CL_SUCCESS == status) && (CL_SUCCESS == binaryStatus)) {
                            std::cout << "Creation of program from blob read from file, was successful\n";
                        }
                        else {
                            printClCreateProgramWithBinary(status, binaryStatus);
                            BOOST_ASSERT_MSG(false, "clCreateProgramWithBinary returned an error-code");
                        }
                }

                SuccessValue ret = SuccessValue::SUCCESS_VAL;

                const char argsStr_basisArr[] = // NOTE not in sync with the 'real' version of this string
#ifdef DEBUG_MODE
                        "-D DEBUG_MODE=1"                                                            " "
#endif
                        "-I ."                                                                       " ";


#ifndef INTEL_OPENCL_DEBUGGING
  #define argsStr argsStr_basisArr // NOTE: in this case argsStr will be char[],
                                   // but using the code below, it will be char*
#else                              // so do not do anything like (sizeof argsStr)
                // We must 1: allocate space for new string
                //         2: copy over argsStr_basisArr
                //         3: append argsStr_firstPartOfSuffix
                //         4: append filename
                //         5: append prologue of "\""

#pragma message (" WARNING: assuming that we are based in C:\\Users\\mb\\Documents\\main ")
                std::cout << ("WARNING: assuming that we are based in C:\\Users\\mb\\Documents\\main" "\n");

                const char argsStr_firstPartOfSuffix[] = "-g -s \"C:/Users/mb/Documents/main/";
                // absence of a trailing space is vital

                // Step 1
                const size_t filename_strlen = strlen(filename);
                char * const argsStr = (BOOST_TYPEOF(argsStr)) alloca(sizeof argsStr_basisArr + sizeof argsStr_firstPartOfSuffix + filename_strlen);
                // we don't want to count the terminating chars from both literals,
                // but we *do* want space for a closing '"', so they balance out: no need to +1 or -1

                // Step 2
                memcpy(argsStr, argsStr_basisArr, sizeof argsStr_basisArr - 1); // -1 because we do not copy terminating char

                // Step 3
                char * copyToHere = argsStr + sizeof argsStr_basisArr - 1; // progress by number of chars we just copied
                memcpy(copyToHere, argsStr_firstPartOfSuffix, sizeof argsStr_firstPartOfSuffix - 1);

                // Step 4
                copyToHere += sizeof argsStr_firstPartOfSuffix - 1; // progress by number of chars we just copied
                memcpy(copyToHere, filename, filename_strlen);


                // Step 5
                copyToHere += filename_strlen;
                *copyToHere = '"';
                ++copyToHere;
                *copyToHere = '\0';

                // printf("<<%s>>\n",argsStr);
#endif

                std::cout << "---\n";
                std::cout << argsStr;
                std::cout << "\n---\n";

                // we must call clBuildProgram even though we're loading from binary
                status = clBuildProgram(programRef, 1 /*1 device only*/, &device_g, argsStr,
                        /*blocking (no callback)*/ NULL, NULL);

                CHECK_CLBUILDPROGRAM(status);


        if (BOOST_UNLIKELY(status != CL_SUCCESS)) {
                // we know the file exists and opened fine
                printError("clBuildProgram", std::begin(bpemArr), std::end(bpemArr), status);
                ret = SuccessValue::FAILURE_VAL;
        }

        return ret;
}

#else // #ifdef DEBUG_MODE ... else
static void initialiseGivenProgram_RELEASE(const char * ptrToMyKernelStr,
                                           size_t myKernelStr_sizeNotIncludingTermination,
                                           cl_program &programRef) {
        cl_int status = !CL_SUCCESS;

        // OpenCL doesn't need to see the terminating '\0'

        programRef = clCreateProgramWithSource(context_g,
                1,
                &ptrToMyKernelStr,
                &myKernelStr_sizeNotIncludingTermination,
                &status); // non-standard return convention for this function
                BOOST_ASSERT((CL_SUCCESS == status));

                // see http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clBuildProgram.html

                status = clBuildProgram(programRef, 1 /*1 device only*/, &device_g, argsStr,
                        /*blocking (no callback)*/ NULL, NULL);


                if (BOOST_UNLIKELY(status != CL_SUCCESS)) {
                        printError("clBuildProgram", std::begin(bpemArr), std::end(bpemArr), status);

                        // we don't know whether #include'd file even exists
                        std::cerr << "(Are you running this application from the correct directory?)" "\n";

                        std::cerr << ("\n" "Build log:" "\n");
                        printOpenCLBuildLog(main_program_g,device_g);
                        BOOST_ASSERT_MSG(false,"Unable to complete OpenCL build");
                }
}
#endif // #ifdef DEBUG_MODE ... else ...



/**
 * Initialise all programs, handling errors helpfully.
 * Currently we have just one program: this function
 * exists to handle error output in case of failed build.
 */
static void initialisePrograms() {
#ifdef INTEL_OPENCL_DEBUGGING
  std::cout << ("\n" "Using Intel debugging flags" "\n");
#endif

#ifdef DEBUG_MODE
  if (true) {
      initialiseGivenProgram_DEBUG("main.cl", main_program_g);
  } else {
      initialiseGivenProgram_DEBUG__FROM_BINARY("blob.clo", main_program_g);
  }
#else
            initialiseGivenProgram_RELEASE(Include_char_arr, sizeof Include_char_arr, main_program_g);
#endif
}


/**
 * Initialises the 'top level' OpenCL global variables: device, command-queue, programs.
 * Does not initialise buffers or kernels.
 */
void initialiseTopLevelOpenCLGlobals() {

        cl_int status = !CL_SUCCESS;

        if ( OpenCL_Helpers::searchAllPlatformsForBestDevice( device_g, device_version_g ) == SuccessValue::SUCCESS_VAL ) {
                OpenCL_Helpers::printDeviceInfo(device_g);

                                // oddly, doesn't return status: uses ptr
                                context_g = clCreateContext(NULL, 1, &device_g, NULL, NULL, &status);
                                CHECK_CLCREATECONTEXT( status );


#ifdef CL_VERSION_2_0 // if macro is defined, we're building against at least OpenCL 2.0
#ifdef ENABLE_OPENCL_PROFILING
                                // clCreateCommandQueue was deprecated in OpenCL 2.0
// See https://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/clCreateCommandQueueWithProperties.html
// https://www.khronos.org/message_boards/showthread.php/9722-clCreateCommandQueueWithProperties?p=31446&viewfull=1#post31446
                                // The third argument is rather confusing.
                                // It's not just a single scalar variable,
                                // it's a pointer to a zero-terminated 'list' (array).
                                // The pointer's type is const cl_queue_properties*
                                // Elements of the array come from two enums:
                                // cl_command_queue_info (keys), cl_command_queue_properties (values)
                                // "Heterogeneous Computing with OpenCL 2.0" is wrong on both fronts:
                                // https://goo.gl/7yQws2
                                // Remember that in-order host command-queue is the default

                                // Using a struct risks UB from padding. We *must* use an array:
                                 static const cl_queue_properties cqps[] = {
                                     CL_QUEUE_PROPERTIES,       // key 1
                                     CL_QUEUE_PROFILING_ENABLE, // value 1

                                     // for device queues only, though AMD OpenCL won't object with a host queue
                                     // CL_QUEUE_SIZE,          // key 2
                                     // 8,                      // value 2
                                     0 // termination
                                 };
                                 commandQueue_g = clCreateCommandQueueWithProperties(context_g, device_g, cqps, &status);
                                 BOOST_ASSERT_MSG((CL_SUCCESS == status), "clCreateCommandQueueWithProperties returned an error code");
#else // #ifdef ENABLE_OPENCL_PROFILING
                                commandQueue_g = clCreateCommandQueueWithProperties(context_g, device_g, NULL, &status);
                                BOOST_ASSERT_MSG((CL_SUCCESS == status), "clCreateCommandQueueWithProperties returned an error code");
#endif // #ifdef ENABLE_OPENCL_PROFILING ... #else ...


#else // #ifdef CL_VERSION_2_0
#ifdef ENABLE_OPENCL_PROFILING
                                commandQueue_g = clCreateCommandQueue(context_g, device_g, CL_QUEUE_PROFILING_ENABLE, &status);
                                BOOST_ASSERT_MSG((CL_SUCCESS == status), "clCreateCommandQueue returned an error code");
#else // #ifdef ENABLE_OPENCL_PROFILING
                                commandQueue_g = clCreateCommandQueue(context_g, device_g, 0, &status);
                                BOOST_ASSERT_MSG((CL_SUCCESS == status), "clCreateCommandQueue returned an error code");
#endif // #ifdef ENABLE_OPENCL_PROFILING ... #else ...

#endif // #ifdef CL_VERSION_2_0 ... #else ...

                #pragma message (" Note: we are using an in-order command-queue ")

        } else {
#if   (defined FORCE_USE_OF_CPU_DEVICE)
                std::cerr << ("No CPU device found" "\n");
#elif (defined FORCE_USE_OF_GPU_DEVICE)
                std::cerr << ("No GPU device found" "\n");
#else
                std::cerr << ("No CPU or GPU device found" "\n");
#endif
                                // Don't use an assert, as it's not a coding-error, it's an environment issue
                                std::cerr << "Terminating.\n";
                                ourExit(10);
        }
        initialisePrograms();
}



void releaseTopLevelOpenCLGlobals() {
                cl_int status = clReleaseProgram(main_program_g);
                BOOST_ASSERT_MSG((CL_SUCCESS == status), "Error doing clReleaseProgram(main_program_g)");

                status = clReleaseCommandQueue(commandQueue_g);
                BOOST_ASSERT_MSG((CL_SUCCESS == status), "Error doing clReleaseCommandQueue(commandQueue_g)");

                status = clReleaseContext(context_g);
                BOOST_ASSERT_MSG((CL_SUCCESS == status), "Error doing clReleaseContext(context_g)");

        // status = clReleaseDevice(device_g); // no need: only for use with OpenCL sub-devices
}
