## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.1.0)

project(ze_raytracing)

INCLUDE(CTest)

SET(RTHWIF_VERSION_MAJOR 4)
SET(RTHWIF_VERSION_MINOR 1)
SET(RTHWIF_VERSION_PATCH 0)
SET(RTHWIF_VERSION ${RTHWIF_VERSION_MAJOR}.${RTHWIF_VERSION_MINOR}.${RTHWIF_VERSION_PATCH})

IF(COMMAND cmake_policy)
  if(POLICY CMP0135)
    message("set policy CMP0135 to NEW")
    cmake_policy(SET CMP0135 NEW)
  endif()
ENDIF()

SET(CMAKE_CXX_STANDARD 17)

ADD_DEFINITIONS("-DZE_RAYTRACING")

SET(EMBREE_CMAKEEXPORT_DIR "cmake")

OPTION(EMBREE_SYCL_IMPLICIT_DISPATCH_GLOBALS "Using L0 allocated Dispatch Globals" ON)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

IF (NOT WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")                       # generate position independent code suitable for shared libraries
ENDIF()

GET_FILENAME_COMPONENT(SYCL_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem \"${SYCL_COMPILER_DIR}/../include/sycl\" -isystem \"${SYCL_COMPILER_DIR}/../include/\"")       # disable warning from SYCL header (FIXME: why required?)

IF (NOT EMBREE_SYCL_IMPLICIT_DISPATCH_GLOBALS)
  ADD_DEFINITIONS("-DEMBREE_SYCL_ALLOC_DISPATCH_GLOBALS")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-bitwise-instead-of-logical") # disables "use of bitwise '&' with boolean operands" warning
SET(CMAKE_CXX_FLAGS_SYCL "${CMAKE_CXX_FLAGS_SYCL} -Wno-bitwise-instead-of-logical") # disables "use of bitwise '&' with boolean operands" warning

include(fetchtbb)
include(package)
INCLUDE(CPack)

IF (EMBREE_SYCL_RT_VALIDATION_API)
  ADD_SUBDIRECTORY(rttrace)
  SET(EMBREE_RTHWIF_SYCL embree_rthwif_sycl)
ENDIF()

ADD_SUBDIRECTORY(rtbuild)

ADD_SUBDIRECTORY(testing)


