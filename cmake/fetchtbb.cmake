## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

####################################################################
# fetch TBB and build static version of it

IF (NOT ZE_RAYTRACING_TBB_GIT_REPOSITORY) # allow setting this externally
  SET(ZE_RAYTRACING_TBB_GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB.git")
ENDIF()

IF (WIN32)
  option(TBB_STRICT "Treat compiler warnings as errors" OFF)
ENDIF()
option(TBB_TEST "Enable testing" OFF)
option(TBBMALLOC_BUILD "Enable tbbmalloc build" OFF)
SET(TBB_DIR OFF)
SET(BUILD_SHARED_LIBS OFF)

# handle tbb directory with highest priority
IF (EXISTS ${PROJECT_SOURCE_DIR}/tbb)
  IF (NOT TBB_HEADER)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tbb ${CMAKE_BINARY_DIR}/tbb EXCLUDE_FROM_ALL)
  ELSE()
    SET(ZE_RAYTRACING_TBB_HEADER_DIR "${PROJECT_SOURCE_DIR}/tbb/include")
  ENDIF()

# next check if TBB headers are to be used and are present
ELSEIF (TBB_HEADER AND EXISTS ${PROJECT_SOURCE_DIR}/external/tbb/${ZE_RAYTRACING_TBB_VERSION})
  MESSAGE(STATUS "Headers for TBB ${ZE_RAYTRACING_TBB_VERSION} are available.")
  SET(ZE_RAYTRACING_TBB_HEADER_DIR "${PROJECT_SOURCE_DIR}/external/tbb/${ZE_RAYTRACING_TBB_VERSION}/include") 

# otherwise download TBB
ELSE()

INCLUDE(FetchContent)

SET(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  tbb_static
  GIT_REPOSITORY ${ZE_RAYTRACING_TBB_GIT_REPOSITORY}
  GIT_TAG ${ZE_RAYTRACING_TBB_VERSION}
)

FetchContent_GetProperties(tbb_static)
if(NOT tbb_static_POPULATED)
  FetchContent_Populate(tbb_static)

  # patch clang.cmake after fetch as WA for https://github.com/uxlfoundation/oneTBB/issues/1741
  if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND 
     ZE_RAYTRACING_TBB_VERSION VERSION_GREATER_EQUAL "v2021.10.0" AND
     ZE_RAYTRACING_TBB_VERSION VERSION_LESS_EQUAL "v2022.3.0")
    file(READ "${tbb_static_SOURCE_DIR}/cmake/compilers/Clang.cmake" TBB_CLANG_CONTENT)
    string(REPLACE "-fPIC" "" TBB_CLANG_CONTENT "${TBB_CLANG_CONTENT}")
    file(WRITE "${tbb_static_SOURCE_DIR}/cmake/compilers/Clang.cmake" "${TBB_CLANG_CONTENT}")
  endif()
  
  # We want to build tbb_static to link it into embree_rthwif, but don't want to
  # install it as part of the Embree install targets.
  IF (NOT TBB_HEADER)
    add_subdirectory(${tbb_static_SOURCE_DIR} ${tbb_static_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Suppress stringop-overflow warnings for TBB build
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(tbb PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wno-stringop-overflow>
      )
    endif()
  ELSE()
    SET(ZE_RAYTRACING_TBB_HEADER_DIR ${tbb_static_SOURCE_DIR}/include)
  ENDIF()
endif()

ENDIF()

MARK_AS_ADVANCED(FETCHCONTENT_BASE_DIR)
MARK_AS_ADVANCED(FETCHCONTENT_FULLY_DISCONNECTED)
MARK_AS_ADVANCED(FETCHCONTENT_QUIET)
MARK_AS_ADVANCED(FETCHCONTENT_SOURCE_DIR_TBB_STATIC)
MARK_AS_ADVANCED(FETCHCONTENT_UPDATES_DISCONNECTED)
MARK_AS_ADVANCED(FETCHCONTENT_UPDATES_DISCONNECTED_TBB_STATIC)

MARK_AS_ADVANCED(TBB4PY_BUILD)
MARK_AS_ADVANCED(TBBMALLOC_BUILD)
MARK_AS_ADVANCED(TBB_BUILD)
MARK_AS_ADVANCED(TBB_CPF)
MARK_AS_ADVANCED(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH)
MARK_AS_ADVANCED(TBB_ENABLE_IPO)
MARK_AS_ADVANCED(TBB_EXAMPLES)
MARK_AS_ADVANCED(TBB_FIND_PACKAGE)
MARK_AS_ADVANCED(TBB_INSTALL_VARS)
MARK_AS_ADVANCED(TBB_NO_APPCONTAINER)
MARK_AS_ADVANCED(TBB_SANITIZE)
MARK_AS_ADVANCED(TBB_STRICT)
MARK_AS_ADVANCED(TBB_TEST)
MARK_AS_ADVANCED(TBB_TEST_SPEC)
MARK_AS_ADVANCED(TBB_VALGRIND_MEMCHECK)
MARK_AS_ADVANCED(TBB_WINDOWS_DRIVER)

