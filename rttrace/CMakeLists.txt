## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

GET_FILENAME_COMPONENT(SYCL_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem \"${SYCL_COMPILER_DIR}/../include/sycl\" -isystem \"${SYCL_COMPILER_DIR}/../include/\"")       # disable warning from SYCL header (FIXME: why required?)

ADD_LIBRARY(embree_rthwif_sycl STATIC rttrace/rthwif_production.cpp)
SET_PROPERTY(TARGET embree_rthwif_sycl APPEND PROPERTY COMPILE_FLAGS "-DEMBREE_SYCL_SUPPORT")
SET_TARGET_PROPERTIES(embree_rthwif_sycl PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_SYCL})

INSTALL(TARGETS embree_rthwif_sycl EXPORT embree_rthwif_sycl-targets ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib)
INSTALL(EXPORT embree_rthwif_sycl-targets DESTINATION "${EMBREE_CMAKEEXPORT_DIR}" COMPONENT devel)

