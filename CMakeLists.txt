CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
## Probably works fine on older versions.


PROJECT(main)

SET (Main_VERSION_MAJOR 1) # not "main..."?
SET (Main_VERSION_MINOR 0)

# SET(BOOST_ROOT "${PROJECT_SOURCE_DIR}/include/boost_1_55_0/myinstall")

SET(Boost_USE_MULTITHREAD OFF)   ## from http://stackoverflow.com/q/7855174
## see www.cmake.org/cmake/help/git-master/module/FindBoost.html



## These two lines may be useful if CMake is unable to find Boost:
# SET(Boost_DEBUG ON)
# SET(Boost_DETAILED_FAILURE_MSG ON)


IF( WIN32  AND  (NOT CYGWIN) )
  SET(Boost_USE_STATIC_LIBS ON) ## there's also Boost_USE_STATIC_RUNTIME, which does.... something else?
  message( "Note: attempting to use static linking for Boost" )
endif()


SET(Boost_MIN_VERSION "1.55.0")
FIND_PACKAGE(Boost ${Boost_MIN_VERSION} REQUIRED) #  COMPONENTS  program_options)
#SET(Boost_EXACT_VERSION "1.55.0")
#FIND_PACKAGE(Boost ${Boost_EXACT_VERSION} EXACT REQUIRED  COMPONENTS  program_options) ## if this fails, CMake will print an error message and then bail
## Things like static_assert and type_traits aren't 'components', only libraries with binaries:
# system filesystem program_options



SET(OpenCL_MIN_VERSION 1.2)
FIND_PACKAGE(OpenCL ${OpenCL_MIN_VERSION} REQUIRED) ## we omit 'EXACT' to request 'at least'

INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIRS} )



## These two lines can resolve Boost linking issues in Windows builds.
## They can also cause mysterious linking issues, however.
## Based on https://stackoverflow.com/a/6624619
# ADD_DEFINITIONS( -DBOOST_ALL_NO_LIB   )
# ADD_DEFINITIONS( -DBOOST_ALL_DYN_LINK )


INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}) ## Boost, *NOT* BOOST
LINK_DIRECTORIES   (${Boost_LIBRARY_DIRS}) ## Boost, *NOT* BOOST

# INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/my_extra_includes")


if( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" ) ## from http://stackoverflow.com/a/7082160
    message( "INFO: DEBUG BUILD" )
#   # for debug type builds, turn on verbose makefiles
#   SET(CMAKE_VERBOSE_MAKEFILE ON)

    # Tell other CMake files that we're doing a debug build
#   SET( DEBUG_BUILD 1 )

    # Define C/C++ macro to express that we're doing a debug build
    ADD_DEFINITIONS( -DDEBUG_MODE )
endif()



#if(CMAKE_COMPILER_IS_GNUCC)              ## from http://stackoverflow.com/a/3818084
#  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -fno-exceptions")     ## from http://web.cs.swarthmore.edu/~adanner/tips/cmake.php
#

#  if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")     ## from http://www.cmake.org/pipermail/cmake/2012-March/049621.html
#  endif()
#
#endif()


if(CMAKE_COMPILER_IS_GNUCXX) ## TODO check for recent g++ before asking for c++11
  ## need exceptions and RTTI for some libs, such as Boost multiprecision

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.4)  ## see http://stackoverflow.com/a/14934542
    if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
    endif()

    set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wextra                           -std=c++11")
#   set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-exceptions -fno-rtti -std=c++11")
  else()
    set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wextra                                     ")
  endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") ## from http://stackoverflow.com/a/10055571

  set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wextra                           -std=c++11")
# set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-exceptions -fno-rtti -std=c++11")

## We don't attempt link-time optimisation with Clang. Doesn't work on Ubuntu - can't find libGold
endif()



message("OpenCL libraries: ${OpenCL_LIBRARIES}")
message("OpenCL include dirs: ${OpenCL_INCLUDE_DIRS}")
message("Boost libraries: ${Boost_LIBRARIES}")
message("Boost include dirs: ${Boost_INCLUDE_DIRS}")

ADD_EXECUTABLE(main
 ${PROJECT_SOURCE_DIR}/main.cpp
 ${PROJECT_SOURCE_DIR}/openclGlobals.cpp
 ${PROJECT_SOURCE_DIR}/openclHelpers.cpp
 ${PROJECT_SOURCE_DIR}/errorHandling.cpp

 ${PROJECT_SOURCE_DIR}/cl_macros.h
 ${PROJECT_SOURCE_DIR}/main.hpp
 ${PROJECT_SOURCE_DIR}/openclHelpers.hpp
 ${PROJECT_SOURCE_DIR}/errorHandling.hpp
 ${PROJECT_SOURCE_DIR}/openclGlobals.hpp
 ${PROJECT_SOURCE_DIR}/projAsserts.hpp
)

#if(CROSS_COMPILE_FOR_ANDROID)
#  TARGET_LINK_LIBRARIES(main
#  #   ${Boost_FILESYSTEM_LIBRARY}
#  #   ${Boost_SYSTEM_LIBRARY}
#  #   ${Boost_PROGRAM_OPTIONS_LIBRARY}
#      ${Boost_LIBRARIES}
#      ${OpenCL_LIBRARIES}
#      /home/mb/Packages/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/libgnustl_static.a ## NOT thumb version
#      # /home/mb/Packages/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a-hard/libgnustl_static.a ## NOT thumb version
#)
#else()
  TARGET_LINK_LIBRARIES(main
  #   ${Boost_FILESYSTEM_LIBRARY}
  #   ${Boost_SYSTEM_LIBRARY}
  #   ${Boost_PROGRAM_OPTIONS_LIBRARY}
  #   ${Boost_LIBRARIES}
      ${OpenCL_LIBRARIES}
  )

