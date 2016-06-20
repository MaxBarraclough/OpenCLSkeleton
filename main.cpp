// TODO overhaul runtime assertions across the whole project, and especially in this file (ignores ENABLE_ macros)

#include "main.hpp"

#include "openclGlobals.hpp"
#include "openclHelpers.hpp"
#include "errorHandling.hpp"

#include <boost/scope_exit.hpp>

// #include "stdlib.h" // already included by main.hpp
#include <iostream>
#include <fstream>
#include <limits>

#include "projAsserts.hpp" // include this one last


static void doOpenClStuff() {
        std::cerr.imbue(std::locale("")); // comma-separated printing of numbers. https://stackoverflow.com/a/15513823
        std::cout.imbue(std::locale(""));

        initialiseTopLevelOpenCLGlobals();
        initialiseBuffers();
        initialiseKernels();

        // Release OpenCL constructs when this current scope ends.
        // Use "void" because we don't need to capture anything. See:
        BOOST_SCOPE_EXIT(void) {  // http://www.boost.org/doc/libs/1_55_0/libs/scope_exit/doc/html/scope_exit/no_variadic_macros.html
                releaseKernels(); // destroy in reverse order of construction
                releaseBuffers();
                releaseTopLevelOpenCLGlobals();
        } BOOST_SCOPE_EXIT_END


        {
                cl_int status = !CL_SUCCESS; // any value other than CL_SUCCESS will do

                if (dump_binary_blob_gc) {
                    OpenCL_Helpers::dumpBinaryOfProgram(main_program_g);
                }

                size_t globalWorkSz = GLOBAL_WORK_SZ;
                size_t localWorkSz =  WG_SIZE;

                cl_event mainEvent = NULL; // it's a pointer type: NULL not 0
                // enqueue's status output: if not CL_SUCCESS, then don't release mainEvent, as enqueue failed
                const cl_int enqueueMainEventStatus = status = clEnqueueNDRangeKernel(
                        commandQueue_g,
                        main_kernel_g,
                        1,    // work_dim
                        NULL, // global_work_offet. NULL => (0,...)

                        &globalWorkSz, // global_work_size
                        &localWorkSz,  // local_work_size

                        0,    // do not wait for any events
                        NULL, // do not wait for any events
                        &mainEvent); // bind to mainEvent var

                CHECK_CLENQUEUENDRANGEKERNEL( status );

                BOOST_SCOPE_EXIT((&mainEvent) (&enqueueMainEventStatus)) {
                    if (CL_SUCCESS == enqueueMainEventStatus) {// assume: not safe to use != NULL as our check:
                                                               // what if it's an index starting at 0, or if it points to garbage because of a failed operation?
                        cl_int status = clReleaseEvent(mainEvent);
                        BOOST_ASSERT_MSG((CL_SUCCESS == status),"clReleaseEvent returned an error-code");
                        mainEvent = NULL; // going out of scope anyway
                    }
                } BOOST_SCOPE_EXIT_END


                status = clFinish(commandQueue_g);
                BOOST_ASSERT_MSG(( CL_SUCCESS == status ), "clFinish returned an error-code");

                std::cout << "Printing the array:\n";
                {
                    const auto end = std::end(bufferData);
                    for (auto ptr = std::begin(bufferData); ptr != end; ++ptr) {
                        std::cout << *ptr;
                        std::cout << ' ';
                    }
                }
                std::cout << "Done.\n";

                if (BOOST_UNLIKELY(status != CL_SUCCESS)) {
                    std::cerr << "clEnqueueNDRangeKernel failed" "\n";
                    CHECK_CLENQUEUENDRANGEKERNEL(status);
                    ourExit(19);
                }
                else {
#ifdef ENABLE_OPENCL_PROFILING
                    size_t sz = 0;

                    // Now retrieve the actual values
                    cl_ulong startTime = 0, endTime = 0;
                    status = clGetEventProfilingInfo(mainEvent, CL_PROFILING_COMMAND_START, sizeof startTime, &startTime, NULL);
                    BOOST_ASSERT_MSG((CL_SUCCESS == status), "clGetEventProfilingInfo returned an error-code");
                    status = clGetEventProfilingInfo(mainEvent, CL_PROFILING_COMMAND_END, sizeof endTime, &endTime, NULL);
                    BOOST_ASSERT_MSG((CL_SUCCESS == status), "clGetEventProfilingInfo returned an error-code");
                    BOOST_ASSERT_MSG((startTime <= endTime), "Sanity-check failure: time taken to execute kernel must be non-negative");

                    const cl_ulong nanosecondsTaken = endTime - startTime;
                    size_t resolutionInNanoseconds = 0;

                    // Now retrieve actual value
                    status = clGetDeviceInfo(device_g,
                        CL_DEVICE_PROFILING_TIMER_RESOLUTION,
                        sizeof resolutionInNanoseconds,
                        &resolutionInNanoseconds,
                        NULL);

                    BOOST_ASSERT((CL_SUCCESS == status));

                    std::cout << "Time taken to execute main kernel: ";
                    std::cout << nanosecondsTaken;
                    std::cout << " ns" "\n" "Resolution of timing: ";
                    std::cout << resolutionInNanoseconds;
                    std::cout << " ns" "\n\n";
#endif // #ifdef ENABLE_OPENCL_PROFILING
                }
        }
}



int main(int argc, char *argv[]) {
        // OpenCL_Helpers::printAllPlatforms();

        doOpenClStuff();
        std::cout << "Press enter to terminate (twice if necessary)";
        std::cout.flush();
        getchar();
    return EXIT_SUCCESS;
}


//#undef exit
void ourExit(int status) {
        std::cerr.flush();
        std::cout << "Exiting with code ";
        std::cout << status;
        std::cout << "\nPress enter to terminate (twice if necessary)";
        std::cout << std::endl;
        getchar();

        exit(status);
}
