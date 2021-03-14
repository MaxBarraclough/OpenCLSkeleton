#include "main.hpp"

#include "openclHelpers.hpp"

#include "errorHandling.hpp"

#include <boost/scope_exit.hpp>

#include <iostream>
#include <fstream> // basic_ofstream
#include <limits>

#include <stdlib.h> // for malloc
#include <string.h>

#ifdef DEBUG_MODE
  #include <string>
  #include <fstream>
#endif

// We use alloca over VLAs, because MSVC doesn't support VLAs
// (VLAs *would* be preferable, as they obey sane scope rules)

#if (defined _MSC_VER) // && (defined WIN32 || defined _WIN32))
  #include <malloc.h>  // for _alloca http://msdn.microsoft.com/en-us/library/wb1s57t5.aspx
  #define alloca _alloca
#else
  // #include <stdlib.h> // for alloca (this header already included above)
  // http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_3.html#SEC45
#endif


#include "projAsserts.hpp" // include this one last


// If we need a bigger array, free the old one and allocate a new one.
// Does *not* copy contents across.
#define RESEAT_ARRAY_PTR_IFF_TOO_SMALL(PTR,ELEMS_TRACKER,ELEMS_NEW,TYPE)     \
        if ( (ELEMS_NEW) > (ELEMS_TRACKER) ) {                               \
          free(PTR);                                                         \
          PTR = (TYPE *) malloc( (size_t)(ELEMS_NEW) * sizeof(TYPE) );       \
          ELEMS_TRACKER = (ELEMS_NEW);                                       \
        }


// from http://dhruba.name/2012/08/14/opencl-cookbook-listing-all-devices-and-their-critical-attributes/
void OpenCL_Helpers::printAllDevices() {
        // get all platforms
        cl_uint platformCount = 0;
        cl_int status = clGetPlatformIDs(0, NULL, &platformCount);
        CHECK_CLGETPLATFORMIDS(status);
        cl_platform_id * const platforms = (BOOST_TYPEOF(platforms))malloc(sizeof(cl_platform_id) * platformCount);
        status = clGetPlatformIDs(platformCount, platforms, NULL);
        CHECK_CLGETPLATFORMIDS(status);

                // NOT const: may be modified if reallocation is needed
        char * value              = (BOOST_TYPEOF(value))malloc(64);
        size_t allocatedValueSize = 64;

        size_t devicesElemsAllocated = 8;
        cl_device_id* devices        = (BOOST_TYPEOF(devices))malloc(devicesElemsAllocated * sizeof(cl_device_id));

        for (cl_uint i = 0; i < platformCount; i++) {

                // get all devices
                cl_uint deviceCount = 0;
                status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
                BOOST_ASSERT(( CL_SUCCESS == status ));

                RESEAT_ARRAY_PTR_IFF_TOO_SMALL(devices,devicesElemsAllocated,deviceCount,cl_device_id);

                status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);
                BOOST_ASSERT(( CL_SUCCESS == status ));


                // for each device, print critical attributes
                for (cl_uint j = 0; j < deviceCount; j++) {
                        // print device name
                        std::cout << ( "Platform: "); std::cout << i;
                        std::cout << ", device: "; std::cout << j;
                        std::cout << '\n';

                        size_t valueSize = 0;
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != valueSize ));

                        RESEAT_ARRAY_PTR_IFF_TOO_SMALL(value,allocatedValueSize,valueSize,char);

#ifdef ENABLE_RUNTIME_ASSERTS // #ifndef BOOST_DISABLE_ASSERTS
                        valueSize = 0;
#endif
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != valueSize ));
                        std::cout << j+1; std::cout << ". Device: ";
                        std::cout << value; std::cout << '\n';

                        // print hardware device version
#ifdef ENABLE_RUNTIME_ASSERTS
                        valueSize = 0;
#endif
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != valueSize ));

                        RESEAT_ARRAY_PTR_IFF_TOO_SMALL(value,allocatedValueSize,valueSize,char);

                        status = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
                        BOOST_ASSERT(( CL_SUCCESS == status ));

                        std::cout << ' '; std::cout << (j + 1);
                        std::cout << " Hardware version: "; std::cout << value;
                        std::cout << '\n';

                        // print software driver version
#ifdef ENABLE_RUNTIME_ASSERTS
                        valueSize = 0;
#endif
                        status = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != valueSize ));

                        RESEAT_ARRAY_PTR_IFF_TOO_SMALL(value,allocatedValueSize,valueSize,char);

                        status = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
                        BOOST_ASSERT(( CL_SUCCESS == status ));

                        std::cout << ' '; std::cout << (j+1);
                        std::cout << " Software version: "; std::cout << value;

                        // print OpenCL C version supported by compiler for device
#ifdef ENABLE_RUNTIME_ASSERTS // #ifndef BOOST_DISABLE_ASSERTS
                        valueSize = 0;
#endif
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != valueSize ));

                        RESEAT_ARRAY_PTR_IFF_TOO_SMALL(value,allocatedValueSize,valueSize,char);

                        status = clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
                        BOOST_ASSERT(( CL_SUCCESS == status ));

                        std::cout << ' '; std::cout << (j+1);
                        std::cout << " OpenCL C version: "; std::cout << value;


                        // print parallel compute units
                        cl_uint maxComputeUnits = 0;
                        status = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                        BOOST_ASSERT(( CL_SUCCESS == status ));
                        BOOST_ASSERT(( 0 != maxComputeUnits ));

                        std::cout << ' '; std::cout << (j+1);
                        std::cout << "Parallel compute units: "; std::cout << maxComputeUnits;
                }
        }
        free(devices);
        free(value);
        free(platforms);

        std::cout << '\n'; std::cout << '\n';
}



// adapted from:
// http://dhruba.name/2012/08/13/opencl-cookbook-listing-all-platforms-and-their-attributes/

/**
 * Prints information on all platforms, however many that may be
 */
void OpenCL_Helpers::printAllPlatforms() {

        const char* attributeNames[5] =
                { "Name", "Vendor", "Version", "Profile", "Extensions" };

        const cl_platform_info attributeTypes[5] =
                { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };

        const int attributeCount = COUNT_OF(attributeNames);

        // get platform count
        cl_uint platformCount = 0;
        cl_int status = clGetPlatformIDs(0, NULL, &platformCount);
        CHECK_CLGETPLATFORMIDS(status);

        // get all platforms
        cl_platform_id * const platforms = (BOOST_TYPEOF(platforms)) alloca( sizeof(cl_platform_id) * platformCount );
        status = clGetPlatformIDs(platformCount, platforms, NULL);
        CHECK_CLGETPLATFORMIDS(status);

        char* info = (BOOST_TYPEOF(info))malloc(64); // not const, as it may be reseated
        size_t allocatedInfoSize = 64;

        // for each platform print all attributes
        for (cl_uint i = 0; i < platformCount; i++) {
                std::cout << "\n " << ((unsigned int)(i + 1)) << ". Platform \n";

                for (int j = 0; j < attributeCount; j++) {
                        // get platform attribute value size
                        size_t infoSize = 0;
                        status = clGetPlatformInfo(platforms[i], attributeTypes[j], 0, NULL, &infoSize);
                        BOOST_ASSERT((CL_SUCCESS == status));

                        RESEAT_ARRAY_PTR_IFF_TOO_SMALL(info,allocatedInfoSize,infoSize,char);

                        // get platform attribute value
                        status = clGetPlatformInfo(platforms[i], attributeTypes[j], infoSize, info, NULL);
                        BOOST_ASSERT((CL_SUCCESS == status));

                        // printf("  %u.%d %-11s: %s" "\n", (unsigned int)i + 1, j + 1, attributeNames[j], info);

                        std::cout << "  " << i + 1 << "." << j + 1 << ' ';

                        std::cout << std::left; // left justify
                        const auto oldWidth = std::cout.width(11); // by 11 characters
                        std::cout << attributeNames[j];
                        std::cout.width(oldWidth); // reset
                        std::cout << std::right;   // reset

                        std::cout << ": " << info << '\n';
                }
                std::cout << '\n' << '\n';
        }
        free(info);
        // free(platforms); // alloca => automatic deallocation
}



#ifdef DEBUG_MODE
/* convert the kernel file into a C string */    // Based on AMD's code. Works with AMD's CodeXL debugging.
SuccessValue OpenCL_Helpers::convertToString(const char *filename, std::string& outCppStrRef) {
        std::fstream f(filename, (std::fstream::in | std::fstream::binary));

        SuccessValue ret = SuccessValue::FAILURE_VAL;

        if( f.is_open() ) {
                f.seekg(0, std::fstream::end);
                const size_t size = (size_t)f.tellg();
                f.seekg(0, std::fstream::beg);
                char* str = (BOOST_TYPEOF(str))malloc(size+1);
                if(NULL == str) {
                        f.close();
                        ret = SuccessValue::FAILURE_VAL;
                } else {
                        f.read(str, size);
                        f.close();
                        str[size] = '\0';
                        outCppStrRef = str;
                        free( (void*)str );
                        str = NULL;
                        ret = SuccessValue::SUCCESS_VAL;
                }
        } else {
            std::cerr << "Error: failed to open file: ";
            std::cerr << filename;
            std::cerr << '\n';
            ret = SuccessValue::FAILURE_VAL;
        }

        return ret;
}
#endif


static const char char_arr_1d0[] = { '1', '.', '0' };
static const char char_arr_1d1[] = { '1', '.', '1' };
static const char char_arr_1d2[] = { '1', '.', '2' };
static const char char_arr_2d0[] = { '2', '.', '0' };
static const char char_arr_2d1[] = { '2', '.', '1' };
static const char char_arr_2d2[] = { '2', '.', '2' };

static void handleDeviceVersionNumber(cl_device_id selectedDevice, unsigned int &deviceVersionRef_out) {
    char charArr[64]; // no need to initialize
                      // Populate *deviceVersionPtr_out
    size_t sz = 0;
    cl_int status = clGetDeviceInfo(selectedDevice,
        CL_DEVICE_VERSION,
        0,
        NULL,
        &sz);

    BOOST_ASSERT((sz <= sizeof charArr));

    status = clGetDeviceInfo(selectedDevice,
        CL_DEVICE_VERSION,
        sz,
        charArr,
        NULL);

    // the string is assured to *begin* with something like "OpenCL 1.2"
    // (but there might be lots of additional description afterward)

    BOOST_ASSERT_MSG( (0 == memcmp("OpenCL ", charArr, 7)), // compare first 7 chars: "OpenCL " (no terminating char)
        "Incorrectly formatted version information outputted by OpenCL library");

    char * const startOfVersionDataProper = charArr + 7; // skip past first 7 characters, i.e. the "OpenCL " prolog

    if (0 == memcmp(char_arr_1d0, startOfVersionDataProper, 3)) {
        deviceVersionRef_out = 100;
    }
    else
        if (0 == memcmp(char_arr_1d1, startOfVersionDataProper, 3)) {
            deviceVersionRef_out = 110;
        }
        else
            if (0 == memcmp(char_arr_1d2, startOfVersionDataProper, 3)) {
                deviceVersionRef_out = 120;
            }
            else
                if (0 == memcmp(char_arr_2d0, startOfVersionDataProper, 3)) {
                    deviceVersionRef_out = 200;
                }
                else
                    if (0 == memcmp(char_arr_2d1, startOfVersionDataProper, 3)) {
                        deviceVersionRef_out = 210;
                    }
                    else {
                        if (0 == memcmp(char_arr_2d2, startOfVersionDataProper, 3)) {
                            deviceVersionRef_out = 220;
                        }
                        else {
                            // flag with an extreme value, in case we're running without termination on assertion failure
                            deviceVersionRef_out = std::numeric_limits< BOOST_TYPEOF(deviceVersionRef_out) >::max();
                            BOOST_ASSERT_MSG(false, "Device supports an unrecognised OpenCL version number");
                        }

                    }

}

/**
 * Search a given platform for a device of a given kind.
 * If/when we find a device of the specified device-kind, we select it and stop searching.
 *
 * platform specifies which platform we are scanning for appropriate devices
 * desiredDeviceKind specifies for instance CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU
 * selectedDevicePtr_out points to the cl_device_id variable into which we will store the found device
 * (if one is found).
 * Must point to a valid (and mutable) cl_device_id variable.
 *
 * Returns its success, where failure means no devices of desired kind have been found
 */
static SuccessValue searchPlatformForDeviceOfType(cl_platform_id platform, cl_device_type desiredDeviceKind, cl_device_id &selectedDeviceRef_out, unsigned int &deviceVersionRef_out) {

        /* Choose the first device of the desired kind, if one is found at all. Otherwise return FAILURE_VAL. */
        cl_uint numDevices = 0;
        cl_int  status = clGetDeviceIDs(platform, desiredDeviceKind, 0, NULL, &numDevices);

        if (CL_DEVICE_NOT_FOUND == status) {
            BOOST_ASSERT_MSG((false), "The call to clGetDeviceIDs failed with error-code CL_DEVICE_NOT_FOUND");
        } else {
            BOOST_ASSERT_MSG(( CL_SUCCESS == status ), "The call to clGetDeviceIDs failed");
        }

        SuccessValue ret = SuccessValue::FAILURE_VAL;
        if (0 == numDevices) { // no device of desired kind is available
                std::cout << ("No device of desired kind available on this platform" "\n");
                ret = SuccessValue::FAILURE_VAL;
        } else { // we have found a device of the desired kind
                std::cout << ("Device of desired kind selected" "\n");
                status = clGetDeviceIDs(platform, desiredDeviceKind, 1 /*disregard all but the first*/, &selectedDeviceRef_out/*scalar, not array*/, NULL);
                BOOST_ASSERT(( CL_SUCCESS == status ));

                handleDeviceVersionNumber(selectedDeviceRef_out, deviceVersionRef_out);

                ret = SuccessValue::SUCCESS_VAL;
        }

        return ret;
}



static int findOpenCLVersionForDevice( const cl_device_id currentDevice ) {
// Yes, we have to mess about with strings, see
// https://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/clGetDeviceInfo.html
        size_t strSize = 0;
        cl_int status =  clGetDeviceInfo( currentDevice, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &strSize );
        BOOST_ASSERT((CL_SUCCESS == status));
        BOOST_ASSERT((0 != strSize));

        //                             0123456789
        // String goes something like "OpenCL C 1.2 Intel SDK"
        // The spec dictates this format.

        // including termination, must be at least 15, but AMD leaves off the vendor info, so 13
        BOOST_ASSERT((strSize >= 13));
        char * const str = (BOOST_TYPEOF(str)) alloca( strSize /* * sizeof(char) */ );
        status = clGetDeviceInfo( currentDevice, CL_DEVICE_OPENCL_C_VERSION, strSize, str, NULL );
        BOOST_ASSERT((CL_SUCCESS == status));

        BOOST_ASSERT((  '.' == str[10]  ));
        BOOST_ASSERT((  str[9] >= '0'  ));
        BOOST_ASSERT((  str[9] <= '9'  ));
        BOOST_ASSERT((  str[11] >= '0'  ));
        BOOST_ASSERT((  str[11] <= '9'  ));

        const int majorVersion = (int)(str[9] - '0');
        const int minorVersion = (int)(str[11] - '0');
        const int ret = (majorVersion * 100) + (minorVersion * 10);
        return ret;
}

static int findOpenCLVersionForPlatform( const cl_platform_id currentPlatform ) {
// Yes, we have to mess about with strings, see
// https://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/clGetPlatformInfo.html
        size_t strSize = 0;
        cl_int status  = clGetPlatformInfo( currentPlatform, CL_PLATFORM_VERSION, 0, NULL, &strSize );
        BOOST_ASSERT((CL_SUCCESS == status));
        BOOST_ASSERT((0 != strSize));

        //                             0123456789
        // String goes something like "OpenCL 1.2 Intel SDK"
        // The spec dictates this format.

        BOOST_ASSERT((strSize >= 13)); // including termination, must be at least 13
        char * const str = (BOOST_TYPEOF(str)) alloca( strSize /* * sizeof(char) */ );
        status = clGetPlatformInfo( currentPlatform, CL_PLATFORM_VERSION, strSize, str, NULL );
        BOOST_ASSERT((CL_SUCCESS == status));

        BOOST_ASSERT((  '.' == str[8]  ));
        BOOST_ASSERT((  str[7] >= '0'  ));
        BOOST_ASSERT((  str[7] <= '9'  ));
        BOOST_ASSERT((  str[9] >= '0'  ));
        BOOST_ASSERT((  str[9] <= '9'  ));

        const int majorVersion = (int)(str[7] - '0');
        const int minorVersion = (int)(str[9] - '0');
        const int ret = (majorVersion * 100) + (minorVersion * 10);
        return ret;
}

#ifdef ENSURE_OPENCL_1_2
  /**
   * Returns "failed?" i.e. true if specified platform supports
   * exactly version 1.2 of OpenCL otherwise returns false
   */
  static bool ensureOpenCL12(cl_platform_id currentPlatform) {
      return ( CL_VERSION_1_2_NUM != findOpenCLVersionForPlatform(currentPlatform) );
  }
  #define ENSURE_CORRECT_OPENCL_VERSION_IF_APPROPRIATE(arg) ensureOpenCL12(arg)
#else
  #define ENSURE_CORRECT_OPENCL_VERSION_IF_APPROPRIATE(arg) true
#endif


void OpenCL_Helpers::printDeviceInfo(cl_device_id device) {
                size_t stringLengthIncTermination = 0;
                cl_int status = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &stringLengthIncTermination);
                BOOST_ASSERT((CL_SUCCESS == status));
                BOOST_ASSERT((0 != stringLengthIncTermination));

                char * deviceDescriptionStr = (BOOST_TYPEOF(deviceDescriptionStr)) alloca( stringLengthIncTermination * sizeof(char) );

                status = clGetDeviceInfo(device, CL_DEVICE_NAME, stringLengthIncTermination, deviceDescriptionStr, NULL);
                BOOST_ASSERT((CL_SUCCESS == status));

                std::cout << "Device selected: <<";
                std::cout << deviceDescriptionStr;
                std::cout << ">>\nfor which the runtime compiler supports OpenCL C version: <<";
                std::cout << findOpenCLVersionForDevice(device);
                std::cout << ">>\n";
}


static void printPlatformInfo_helper(const cl_platform_id currentPlatform, size_t strSize) {
        char * const str = (BOOST_TYPEOF(str)) alloca( strSize /* * sizeof(char) */ );
        cl_int status = clGetPlatformInfo( currentPlatform, CL_PLATFORM_VERSION, strSize, str, NULL );
                BOOST_ASSERT((CL_SUCCESS == status));
                BOOST_ASSERT((  '.' == str[8]  ));

                std::cout << "Platform supports OpenCL version <<";
                std::cout << str;
                std::cout << ">>\n";
}

static void printPlatformInfo(const cl_platform_id currentPlatform) {
// Yes, we have to mess about with strings, see
// https://www.khronos.org/registry/cl/sdk/2.0/docs/man/xhtml/clGetPlatformInfo.html
        size_t strSize = 0;
        cl_int status = clGetPlatformInfo( currentPlatform, CL_PLATFORM_VERSION, 0, NULL, &strSize );
                BOOST_ASSERT((CL_SUCCESS == status));
                BOOST_ASSERT((0 != strSize));

                printPlatformInfo_helper(currentPlatform,strSize);

                BOOST_ASSERT((strSize >= 13)); // including termination, must be at least 13

                status = clGetPlatformInfo( currentPlatform, CL_PLATFORM_NAME, 0, NULL, &strSize );
                BOOST_ASSERT((CL_SUCCESS == status));
                BOOST_ASSERT((0 != strSize));

                char * const str = (BOOST_TYPEOF(str)) alloca( strSize /* * sizeof(char) */ );
                status = clGetPlatformInfo( currentPlatform, CL_PLATFORM_NAME, strSize, str, NULL );
                BOOST_ASSERT((CL_SUCCESS == status));
                std::cout << "Platform name: <<";
                std::cout << str;
                std::cout << ">>\n";
}

/**
 * devicePtr points to the cl_device_id variable into which we will read the id of the 'best' device.
 * We first begin searching for a GPU device, searching each platform in turn.
 * If one is found, we select it, and its id is saved via devicePtr.
 * If not, we search each platform for a CPU device, and its id is saved via devicePtr
 *
 * 'true' is returned iff a GPU or CPU device is eventually found.
 *
 * If no valid devices are found, *devicePtr is never assigned.
 *
 * NOTE: UNTESTED where we are not using the first available platform
 */
SuccessValue OpenCL_Helpers::searchAllPlatformsForBestDevice(cl_device_id &deviceRef_out, unsigned int &deviceVersionRef_out) {
        /* Step 1: Get platforms and choose an available one. */
        cl_uint numPlatforms = 0;        // number of platforms
        cl_int        status = clGetPlatformIDs(0, NULL, &numPlatforms);
        CHECK_CLGETPLATFORMIDS(status);

        SuccessValue ret = SuccessValue::FAILURE_VAL;

        if (0 == numPlatforms) {
                // Abort and fail if (numPlatforms == 0)
                std::cerr << ("Error: zero OpenCL platforms" "\n");
                ret = SuccessValue::FAILURE_VAL;
        } else {
                cl_platform_id * const platforms = (BOOST_TYPEOF(platforms)) alloca( numPlatforms * sizeof(cl_platform_id) );
                status = clGetPlatformIDs(numPlatforms, platforms, NULL);
                CHECK_CLGETPLATFORMIDS(status);

                bool allDone = false;
                cl_uint platformIndex = std::numeric_limits<cl_uint>::max();

                // Search for a GPU (GPU is first choice, if none found then look for a CPU)
#ifdef FORCE_USE_OF_CPU_DEVICE // forcing CPU means bypass check for GPU device
                if(false)
#endif
                for ( cl_uint iii = 0; !allDone && (iii != numPlatforms); ++iii) {
                        const cl_platform_id currentPlatform = platforms[iii];
                        if ( ENSURE_CORRECT_OPENCL_VERSION_IF_APPROPRIATE(currentPlatform) ) {
                                const bool deviceFound =
                                    ( searchPlatformForDeviceOfType(
                                        currentPlatform,
                                        CL_DEVICE_TYPE_GPU,
                                        deviceRef_out,
                                        deviceVersionRef_out)
                                        == SuccessValue::SUCCESS_VAL );

                                if ( deviceFound ) {
                                        // we have a match, and devicePtr holds the correct value
                                        platformIndex = iii;
                                        allDone = true;
                                        ret = SuccessValue::SUCCESS_VAL;
                                }
                        }
                }


                // Search for a CPU
#ifdef FORCE_USE_OF_GPU_DEVICE // forcing GPU means bypass check for CPU device
                if(false)
#endif
                for ( cl_uint jjj = 0; !allDone && (jjj != numPlatforms); ++jjj) {
                        const cl_platform_id currentPlatform_ = platforms[jjj];
                        if( ENSURE_CORRECT_OPENCL_VERSION_IF_APPROPRIATE(currentPlatform_) ) {
                                const bool deviceFound_ =
                                    ( searchPlatformForDeviceOfType(
                                        currentPlatform_,
                                        CL_DEVICE_TYPE_CPU,
                                        deviceRef_out,
                                        deviceVersionRef_out)
                                        == SuccessValue::SUCCESS_VAL );

                                if ( deviceFound_ ) {
                                        platformIndex = jjj;
                                        allDone = true;
                                        ret = SuccessValue::SUCCESS_VAL; // we have a match, and devicePtr holds the correct value
                                }
                        }
                }

                printPlatformInfo( platforms[platformIndex] );
        }
        return ret;
}



void OpenCL_Helpers::dumpBinaryOfProgram(cl_program program)
    {
        // Ensure we have exactly 1 device, i.e. 1 binary of our program (these are equivalent)
        cl_uint numDevices = 0;
        cl_int status = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof numDevices, &numDevices, NULL);
        CHECK_CLGETPROGRAMINFO(status);
        BOOST_ASSERT_MSG((1 == numDevices), "There should be exactly 1 device");

        // Now fetch the size of the (one and only) binary
        size_t binSz = 0;
        status = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof binSz, &binSz, NULL);
        CHECK_CLGETPROGRAMINFO(status);

        unsigned char * binaryBlobArr = (BOOST_TYPEOF(binaryBlobArr))/*malloc(sz)*/calloc(1, binSz);
        BOOST_ASSERT_MSG((NULL != binaryBlobArr),"calloc failed to allocate memory");

        BOOST_SCOPE_EXIT((&binaryBlobArr)) {
            BOOST_ASSERT((NULL != binaryBlobArr));
            free((void*)binaryBlobArr);
            binaryBlobArr = NULL;
        } BOOST_SCOPE_EXIT_END

        // Retrieve the one and only CL_PROGRAM_BINARIES output
        // Note that the sizeof arg is for the 'out' pointer itself, *not* for the allocated block
        status = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char**), (void*)&binaryBlobArr, NULL);
        CHECK_CLGETPROGRAMINFO(status);

        typedef std::basic_ofstream<unsigned char> uofstream; // like ofstream, but for unsigned char not char
        uofstream blobOutStream("blob.clo", uofstream::binary); // TODO file-name shouldn't be hard-coded
        blobOutStream.write(binaryBlobArr, binSz);
        blobOutStream.close(); // throws if no stream open

        std::cout << "Binary file has been written. Size: ";
        std::cout << binSz;
        std::cout << " bytes";
        std::cout << std::endl;
    }
