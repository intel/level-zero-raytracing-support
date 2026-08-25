// Copyright 2009-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <iostream>

#include "level_zero/ze_wrapper.h"

#if defined(__cplusplus)
#  define RTHWIF_API_EXTERN_C extern "C"
#else
#  define RTHWIF_API_EXTERN_C
#endif

#if defined(_WIN32)
#if defined(EMBREE_RTHWIF_STATIC_LIB)
#  define RTHWIF_API_IMPORT RTHWIF_API_EXTERN_C
#  define RTHWIF_API_EXPORT RTHWIF_API_EXTERN_C
#else
#  define RTHWIF_API_IMPORT RTHWIF_API_EXTERN_C __declspec(dllimport)
#  define RTHWIF_API_EXPORT RTHWIF_API_EXTERN_C __declspec(dllexport)
#endif
#else
#  define RTHWIF_API_IMPORT RTHWIF_API_EXTERN_C
#  define RTHWIF_API_EXPORT RTHWIF_API_EXTERN_C __attribute__ ((visibility ("default")))
#endif

typedef enum _ze_rtas_format_t {
  ZE_RTAS_FORMAT_INVALID = 0,   // invalid acceleration structure format
  ZE_RTAS_FORMAT_DG2 = 1,       // acceleration structure format for DG2
  ZE_RTAS_FORMAT_PTL = 2,       // acceleration structure format for PTL
  ZE_RTAS_FORMAT_NVL_P = 3,      // acceleration structure format for NVL-P
  ZE_RTAS_FORMAT_VERSION_4 = 4,
  ZE_RTAS_FORMAT_VERSION_5 = 5,
} ze_rtas_format_t;

inline std::ostream& operator<<(std::ostream& cout, const ze_rtas_format_t& rtas_format)
{
  switch (rtas_format)
  {
  case ZE_RTAS_FORMAT_INVALID: return cout << "ZE_RTAS_FORMAT_INVALID";
  case ZE_RTAS_FORMAT_DG2: return cout << "ZE_RTAS_FORMAT_DG2";
  case ZE_RTAS_FORMAT_PTL: return cout << "ZE_RTAS_FORMAT_PTL";
  case ZE_RTAS_FORMAT_NVL_P: return cout << "ZE_RTAS_FORMAT_NVL_P";
  case ZE_RTAS_FORMAT_VERSION_4: return cout << "ZE_RTAS_FORMAT_VERSION_4";
  case ZE_RTAS_FORMAT_VERSION_5: return cout << "ZE_RTAS_FORMAT_VERSION_5";
  default: return cout << "ZE_RTAS_FORMAT_UNKNOWN_" << (uint32_t) rtas_format;
  }
}

inline bool valid_rtas_format(ze_rtas_format_t rtas_format)
{
  bool valid_rtas_format = false;
  valid_rtas_format |= (rtas_format == ZE_RTAS_FORMAT_DG2);
  valid_rtas_format |= (rtas_format == ZE_RTAS_FORMAT_PTL);
  valid_rtas_format |= (rtas_format == ZE_RTAS_FORMAT_NVL_P);
  valid_rtas_format |= (rtas_format == ZE_RTAS_FORMAT_VERSION_4);
  valid_rtas_format |= (rtas_format == ZE_RTAS_FORMAT_VERSION_5);
  return valid_rtas_format;
}

inline bool has_old_instance_layout(ze_rtas_format_t rtas_format) {
  return rtas_format == ZE_RTAS_FORMAT_DG2;
}


/* EXT version of API */
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderCreateExtImpl(ze_driver_handle_t hDriver, const ze_rtas_builder_ext_desc_t *pDescriptor, ze_rtas_builder_ext_handle_t *phBuilder);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderDestroyExtImpl(ze_rtas_builder_ext_handle_t hBuilder);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeDriverRTASFormatCompatibilityCheckExtImpl( ze_driver_handle_t hDriver,
                                                                                       const ze_rtas_format_ext_t accelFormat,
                                                                                       const ze_rtas_format_ext_t otherAccelFormat);
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderGetBuildPropertiesExtImpl(ze_rtas_builder_ext_handle_t hBuilder,
                                                                                const ze_rtas_builder_build_op_ext_desc_t* args,
                                                                                ze_rtas_builder_ext_properties_t* pProp);
  
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderBuildExtImpl(ze_rtas_builder_ext_handle_t hBuilder,
                                                                   const ze_rtas_builder_build_op_ext_desc_t* args,
                                                                   void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                                                   void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                                                   ze_rtas_parallel_operation_ext_handle_t hParallelOperation,
                                                                   void *pBuildUserPtr, ze_rtas_aabb_ext_t *pBounds, size_t *pRtasBufferSizeBytes);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationCreateExtImpl(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_ext_handle_t* phParallelOperation);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationDestroyExtImpl( ze_rtas_parallel_operation_ext_handle_t hParallelOperation );

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationGetPropertiesExtImpl( ze_rtas_parallel_operation_ext_handle_t hParallelOperation, ze_rtas_parallel_operation_ext_properties_t* pProperties );

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationJoinExtImpl( ze_rtas_parallel_operation_ext_handle_t hParallelOperation);


/* EXP version of API */
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderCreateExpImpl(ze_driver_handle_t hDriver, const ze_rtas_builder_exp_desc_t *pDescriptor, ze_rtas_builder_exp_handle_t *phBuilder);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderDestroyExpImpl(ze_rtas_builder_exp_handle_t hBuilder);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeDriverRTASFormatCompatibilityCheckExpImpl( ze_driver_handle_t hDriver,
                                                                                       const ze_rtas_format_exp_t accelFormat,
                                                                                       const ze_rtas_format_exp_t otherAccelFormat);
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderGetBuildPropertiesExpImpl(ze_rtas_builder_exp_handle_t hBuilder,
                                                                                const ze_rtas_builder_build_op_exp_desc_t* args,
                                                                                ze_rtas_builder_exp_properties_t* pProp);
  
RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASBuilderBuildExpImpl(ze_rtas_builder_exp_handle_t hBuilder,
                                                                   const ze_rtas_builder_build_op_exp_desc_t* args,
                                                                   void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                                                   void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                                                   ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                                                                   void *pBuildUserPtr, ze_rtas_aabb_exp_t *pBounds, size_t *pRtasBufferSizeBytes);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationCreateExpImpl(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_exp_handle_t* phParallelOperation);

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationDestroyExpImpl( ze_rtas_parallel_operation_exp_handle_t hParallelOperation );

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationGetPropertiesExpImpl( ze_rtas_parallel_operation_exp_handle_t hParallelOperation, ze_rtas_parallel_operation_exp_properties_t* pProperties );

RTHWIF_API_EXPORT ze_result_t ZE_APICALL zeRTASParallelOperationJoinExpImpl( ze_rtas_parallel_operation_exp_handle_t hParallelOperation);
