// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ze_api.h"

/* API Type */
enum API_TY {
  EXT_API = 0,
  EXP_API = 1
};

//////////////////////
// Debug extension

#define ZE_STRUCTURE_TYPE_RTAS_BUILDER_BUILD_OP_DEBUG_DESC ((ze_structure_type_t)0x00020020)  ///< ::ze_rtas_builder_build_op_debug_desc_t

typedef struct _ze_rtas_builder_build_op_debug_desc_t
{
  ze_structure_type_t stype;                                              ///< [in] type of this structure
  const void* pNext;                                                      ///< [in][optional] must be null or a pointer to an extension-specific
                                                                          ///< structure (i.e. contains stype and pNext).
  void* dispatchGlobalsPtr;
  
} ze_rtas_builder_build_op_debug_desc_t;

////////////////////

struct ZeWrapper
{
  enum RTAS_BUILD_MODE {
    INVALID = 0,    
    INTERNAL = 1,   // use internal RTAS build implementation
    LEVEL_ZERO = 2, // use Level Zero provided RTAS build implementation
  };

  ~ZeWrapper();

  static ze_result_t init();
  static ze_result_t initRTASBuilder(API_TY aty, ze_driver_handle_t hDriver, RTAS_BUILD_MODE rtas_build_mode);
  
  static ze_result_t zeMemFree(ze_context_handle_t, void*);
  static ze_result_t zeMemAllocShared(ze_context_handle_t, const ze_device_mem_alloc_desc_t*, const ze_host_mem_alloc_desc_t*, size_t, size_t, ze_device_handle_t, void**);
  static ze_result_t zeDriverGetExtensionProperties(ze_driver_handle_t, uint32_t*, ze_driver_extension_properties_t*);
  static ze_result_t zeDeviceGetProperties(ze_device_handle_t, ze_device_properties_t*);
  static ze_result_t zeDeviceGetModuleProperties(ze_device_handle_t, ze_device_module_properties_t*);

  /* EXP version of API */
  static ze_result_t zeRTASBuilderCreateExp(ze_driver_handle_t hDriver, const ze_rtas_builder_exp_desc_t *pDescriptor, ze_rtas_builder_exp_handle_t *phBuilder);
  static ze_result_t zeRTASBuilderDestroyExp(ze_rtas_builder_exp_handle_t hBuilder);
  static ze_result_t zeDriverRTASFormatCompatibilityCheckExp( ze_driver_handle_t hDriver,
                                                              const ze_rtas_format_exp_t accelFormat,
                                                              const ze_rtas_format_exp_t otherAccelFormat);
  static ze_result_t zeRTASBuilderGetBuildPropertiesExp(ze_rtas_builder_exp_handle_t hBuilder,
                                                        const ze_rtas_builder_build_op_exp_desc_t* args,
                                                        ze_rtas_builder_exp_properties_t* pProp);
  static ze_result_t zeRTASBuilderBuildExp(ze_rtas_builder_exp_handle_t hBuilder,
                                           const ze_rtas_builder_build_op_exp_desc_t* args,
                                           void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                           void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                           ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                                           void *pBuildUserPtr, ze_rtas_aabb_exp_t *pBounds, size_t *pRtasBufferSizeBytes);

  static ze_result_t zeRTASBuilderCommandListAppendCopyExp(ze_command_list_handle_t hCommandList,
                                                           void* dstptr,
                                                           const void* srcptr,
                                                           size_t size,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t* phWaitEvents);
  
  static ze_result_t zeRTASParallelOperationCreateExp(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_exp_handle_t* phParallelOperation);
  static ze_result_t zeRTASParallelOperationDestroyExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation );
  static ze_result_t zeRTASParallelOperationGetPropertiesExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation, ze_rtas_parallel_operation_exp_properties_t* pProperties );
  static ze_result_t zeRTASParallelOperationJoinExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation);

  /* EXT version of API */
  static ze_result_t zeRTASBuilderCreateExt(ze_driver_handle_t hDriver, const ze_rtas_builder_ext_desc_t *pDescriptor, ze_rtas_builder_ext_handle_t *phBuilder);
  static ze_result_t zeRTASBuilderDestroyExt(ze_rtas_builder_ext_handle_t hBuilder);
  static ze_result_t zeDriverRTASFormatCompatibilityCheckExt( ze_driver_handle_t hDriver,
                                                              const ze_rtas_format_ext_t accelFormat,
                                                              const ze_rtas_format_ext_t otherAccelFormat);
  static ze_result_t zeRTASBuilderGetBuildPropertiesExt(ze_rtas_builder_ext_handle_t hBuilder,
                                                        const ze_rtas_builder_build_op_ext_desc_t* args,
                                                        ze_rtas_builder_ext_properties_t* pProp);
  static ze_result_t zeRTASBuilderBuildExt(ze_rtas_builder_ext_handle_t hBuilder,
                                           const ze_rtas_builder_build_op_ext_desc_t* args,
                                           void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                           void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                           ze_rtas_parallel_operation_ext_handle_t hParallelOperation,
                                           void *pBuildUserPtr, ze_rtas_aabb_ext_t *pBounds, size_t *pRtasBufferSizeBytes);

  static ze_result_t zeRTASBuilderCommandListAppendCopyExt(ze_command_list_handle_t hCommandList,
                                                           void* dstptr,
                                                           const void* srcptr,
                                                           size_t size,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t* phWaitEvents);
  
  static ze_result_t zeRTASParallelOperationCreateExt(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_ext_handle_t* phParallelOperation);
  static ze_result_t zeRTASParallelOperationDestroyExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation );
  static ze_result_t zeRTASParallelOperationGetPropertiesExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation, ze_rtas_parallel_operation_ext_properties_t* pProperties );
  static ze_result_t zeRTASParallelOperationJoinExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation);

  static RTAS_BUILD_MODE rtas_builder;
};

