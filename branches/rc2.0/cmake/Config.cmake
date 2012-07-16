######################################################################################
### 0) Detect the configuration
######################################################################################

# Inspired by Boost and SFML CMake files
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX 1)

    # detect OS X version. (use '/usr/bin/sw_vers -productVersion' to extract V from '10.V.x'.)
    execute_process (COMMAND /usr/bin/sw_vers -productVersion OUTPUT_VARIABLE MACOSX_VERSION_RAW)
    string(REGEX REPLACE "10\\.([0-9]).*" "\\1" MACOSX_VERSION "${MACOSX_VERSION_RAW}")
    if(${MACOSX_VERSION} LESS 5)
        message(FATAL_ERROR "Unsupported version of OS X : ${MACOSX_VERSION_RAW}")
        return()
    endif()
endif()

# Determine architecture
include(CheckTypeSize)
check_type_size(void* SIZEOF_VOID_PTR)
if("${SIZEOF_VOID_PTR}" STREQUAL "4")
    set(ARCH x86)
    set(LIB lib32)
elseif("${SIZEOF_VOID_PTR}" STREQUAL "8")
    set(ARCH x64)
    set(LIB lib64)
else()
    message(FATAL_ERROR "Unsupported architecture")
    return()
endif()


######################################################################################
### 0) Define general CXX flags for DEBUG and RELEASE
######################################################################################

add_definitions(-DDEPRECATED_MESSAGES)
set(CMAKE_CXX_FLAGS_DEBUG  "-Wunknown-pragmas -O0 -g -Wall -Wextra -ansi -pedantic -fopenmp -std=c++0x" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE  "-Wunknown-pragmas -O3 -fopenmp -std=c++0x" CACHE STRING "" FORCE)

######################################################################################
### 1) Define installation type
######################################################################################

if(INSTALL_TYPE STREQUAL full)
    set(ENABLE_CMAKE_EXAMPLE "true" CACHE PATH "ParadisEO examples")
    set(ENABLE_CMAKE_TESTING "true" CACHE PATH "ParadisEO tests")
elseif(INSTALL_TYPE STREQUAL min)
    set(ENABLE_CMAKE_EXAMPLE "false" CACHE PATH "ParadisEO examples")
    set(ENABLE_CMAKE_TESTING "false" CACHE PATH "ParadisEO tests")
endif()

######################################################################################
### 2) Define profiling flags
######################################################################################

if(PROFILING)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg --coverage" CACHE STRING "" FORCE)
        set(ENABLE_CMAKE_TESTING "true" CACHE STRING "" FORCE)
endif(PROFILING)

######################################################################################
### 3) Testing part
######################################################################################

if(ENABLE_CMAKE_TESTING)
    enable_testing()
    include(CTest REQUIRED)
    set(DEBUG "true" CACHE STRING "" FORCE)
endif(ENABLE_CMAKE_TESTING)

######################################################################################
### 4) Set DEBUG or RELEASE build type depending testing and profiling
######################################################################################

if(NOT DEFINED DEBUG)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
else( NOT DEFINED DEBUG OR DEFINED PREFIX )
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif(NOT DEFINED DEBUG)

######################################################################################
### 5) Build examples ?
######################################################################################

set(ENABLE_CMAKE_EXAMPLE "true" CACHE PATH "ParadisEO examples")

######################################################################################
### 6) Determine prefix for installation
######################################################################################

if(UNIX)
    set(INSTALL_SUB_DIR /paradiseo)
endif()