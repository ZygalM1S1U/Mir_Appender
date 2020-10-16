# set min version of cmake required
cmake_minimum_required(VERSION 3.16.2)

project(mirConfig C CXX)

# set macro directory
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/CMakeModules")

# build in release mode by default (if not explicitly set)
if( NOT CMAKE_BUILD_TYPE )
set(CMAKE_BUILD_TYPE "Release")
message(STATUS "Using RELEASE type")
endif()

# set default buildoptions and print them
include(cmake/options.cmake)

# check if 32 or 64 bit
if (CMAKE_SIZEOF_VOID_P MATCHES 8)
set(PLATFORM 64)
MESSAGE(STATUS "Dectected 64-bit platform")
else()
set(PLATFORM 32)
MESSAGE(STATUS "Dectected 32-bit platform")
endif()

if (WIN32)
MESSAGE(STATUS "Looks like you're on windows")
endif()

# newer versions of cmake do not like relative include dirs, so stop warnings for newer cmake versions
if ("${CMAKE_VERSION}" VERSION_GREATER 2.8.12)
    cmake_policy(SET CMP0021 OLD)
endif()

##############################
# Find and setup OpenGL
# 
find_package(OpenGL REQUIRED)

##############################
# Find and setup GLEW
#  note - this is only on Linux, at the bottom of this file is added the glew project directly for windows
if (NOT WIN32)
	find_package(GLEW REQUIRED)
endif()

##############################
# Find and setup SDL2
#
if (NOT WIN32)
    find_package(SDL2 REQUIRED)
endif()

# make sure c++ 11 support on Linux
if (NOT WIN32)
    add_definitions(-std=c++11)
else()
	add_definitions(-DNOMINMAX)  # ridiculous defines in some win include has conflicts with glm functions, this suppresses those
endif()

# Add flag for multiple core compile for windows
if (WIN32)
SET(CMAKE_CXX_FLAGS "/MP /EHsc")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else()
if (CMAKE_BUILD_TYPE MATCHES "Release")
    SET(CMAKE_CXX_FLAGS "-O3")
    message(STATUS "Turning On Optimizations")
endif()
endif()

# add dependency projects directly for windows, we used find package for system install for Linux
if (WIN32)
	add_subdirectory(deps/glew-2.2.0/build/cmake)
endif()


add_subdirectory(src)

include(cmake/showoptions.cmake)

