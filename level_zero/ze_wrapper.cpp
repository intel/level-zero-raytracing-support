// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

/* detect Linux platform */
#if defined(linux) || defined(__linux__) || defined(__LINUX__)
#  if !defined(__LINUX__)
#     define __LINUX__
#  endif
#endif

#if defined(__LINUX__)
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include "ze_wrapper.h"

#include "../rtbuild/rtbuild.h"

#include <iostream>
#include <cstdio>
#include <cassert>
#include <mutex>
#include <string.h>

ZeWrapper::RTAS_BUILD_MODE ZeWrapper::rtas_builder = ZeWrapper::INVALID;

static std::mutex zeWrapperMutex;
static void* handle = nullptr;

static decltype(zeMemFree)* zeMemFreeInternal = nullptr;
static decltype(zeMemAllocShared)* zeMemAllocSharedInternal = nullptr;
static decltype(zeMemAllocDevice)* zeMemAllocDeviceInternal = nullptr;
static decltype(zeDriverGetExtensionProperties)* zeDriverGetExtensionPropertiesInternal = nullptr;
static decltype(zeDeviceGetProperties)* zeDeviceGetPropertiesInternal = nullptr;
static decltype(zeDeviceGetModuleProperties)* zeDeviceGetModulePropertiesInternal = nullptr;
static decltype(zeCommandListAppendMemoryCopy)* zeCommandListAppendMemoryCopyInternal = nullptr;
static decltype(zeCommandQueueExecuteCommandLists)* zeCommandQueueExecuteCommandListsInternal = nullptr;
static decltype(zeCommandListCreate)* zeCommandListCreateInternal = nullptr;
static decltype(zeCommandListClose)* zeCommandListCloseInternal = nullptr;
static decltype(zeCommandListDestroy)* zeCommandListDestroyInternal = nullptr;

/* EXP version of API */
static decltype(zeRTASBuilderCreateExp)* zeRTASBuilderCreateExpInternal = nullptr;
static decltype(zeRTASBuilderDestroyExp)* zeRTASBuilderDestroyExpInternal = nullptr;
static decltype(zeDriverRTASFormatCompatibilityCheckExp)* zeDriverRTASFormatCompatibilityCheckExpInternal = nullptr;
static decltype(zeRTASBuilderGetBuildPropertiesExp)* zeRTASBuilderGetBuildPropertiesExpInternal = nullptr;
static decltype(zeRTASBuilderBuildExp)* zeRTASBuilderBuildExpInternal = nullptr;
  
static decltype(zeRTASParallelOperationCreateExp)* zeRTASParallelOperationCreateExpInternal = nullptr;
static decltype(zeRTASParallelOperationDestroyExp)* zeRTASParallelOperationDestroyExpInternal = nullptr; 
static decltype(zeRTASParallelOperationGetPropertiesExp)* zeRTASParallelOperationGetPropertiesExpInternal = nullptr;
static decltype(zeRTASParallelOperationJoinExp)* zeRTASParallelOperationJoinExpInternal = nullptr;

/* EXT version of API */
static decltype(zeRTASBuilderCreateExt)* zeRTASBuilderCreateExtInternal = nullptr;
static decltype(zeRTASBuilderDestroyExt)* zeRTASBuilderDestroyExtInternal = nullptr;
static decltype(zeDriverRTASFormatCompatibilityCheckExt)* zeDriverRTASFormatCompatibilityCheckExtInternal = nullptr;
static decltype(zeRTASBuilderGetBuildPropertiesExt)* zeRTASBuilderGetBuildPropertiesExtInternal = nullptr;
static decltype(zeRTASBuilderBuildExt)* zeRTASBuilderBuildExtInternal = nullptr;
static decltype(zeRTASBuilderCommandListAppendCopyExt)* zeRTASBuilderCommandListAppendCopyExtInternal = nullptr;
  
static decltype(zeRTASParallelOperationCreateExt)* zeRTASParallelOperationCreateExtInternal = nullptr;
static decltype(zeRTASParallelOperationDestroyExt)* zeRTASParallelOperationDestroyExtInternal = nullptr; 
static decltype(zeRTASParallelOperationGetPropertiesExt)* zeRTASParallelOperationGetPropertiesExtInternal = nullptr;
static decltype(zeRTASParallelOperationJoinExt)* zeRTASParallelOperationJoinExtInternal = nullptr;

template<typename T>
T find_symbol(void* handle, std::string const& symbol) {
#if defined(__LINUX__)
  T result = (T) dlsym(handle, symbol.c_str());
#else
  T result = (T) GetProcAddress((HMODULE)handle, symbol.c_str());
#endif
  if (!result) {
    throw std::runtime_error("level_zero wrapper: symbol " + symbol + " not found");
  }
  return result;
}

void* load_module() {
#if defined(__LINUX__)
  void* handle = dlopen(ZE_LOADER_NAME_LINUX,RTLD_LAZY);
  if (!handle) {
    throw std::runtime_error("module " ZE_LOADER_NAME_LINUX " not found");
  }
#else
  void* handle = LoadLibraryExA(ZE_LOADER_NAME_WINDOWS,NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!handle) {
    throw std::runtime_error("module " ZE_LOADER_NAME_WINDOWS " not found");
  }
#endif
  return handle;
}

void unload_module(void* handle) {
  if (handle) {
#if defined(__LINUX__)
    dlclose(handle);
#else
    FreeLibrary((HMODULE)handle);
#endif
  }
}

ZeWrapper::~ZeWrapper() {
  unload_module(handle);
}

ze_result_t selectLevelZeroRTASBuilderExp(ze_driver_handle_t hDriver)
{
  if (ZeWrapper::rtas_builder == ZeWrapper::LEVEL_ZERO)
    return ZE_RESULT_SUCCESS;

  auto zeRTASBuilderCreateExpTemp = find_symbol<decltype(zeRTASBuilderCreateExp)*>(handle,"zeRTASBuilderCreateExp");
  auto zeRTASBuilderDestroyExpTemp = find_symbol<decltype(zeRTASBuilderDestroyExp)*>(handle,"zeRTASBuilderDestroyExp");
  
  ze_rtas_builder_exp_desc_t builderDesc = { ZE_STRUCTURE_TYPE_RTAS_BUILDER_EXP_DESC };
  ze_rtas_builder_exp_handle_t hBuilder = nullptr;
  ze_result_t err = zeRTASBuilderCreateExpTemp(hDriver, &builderDesc, &hBuilder);

  /* when ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE is reported extension cannot get loaded */
  if (err == ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE)
    return err;

  if (err == ZE_RESULT_SUCCESS)
    zeRTASBuilderDestroyExpTemp(hBuilder);

  zeRTASBuilderCreateExpInternal = zeRTASBuilderCreateExpTemp;
  zeRTASBuilderDestroyExpInternal = zeRTASBuilderDestroyExpTemp;
  
  zeDriverRTASFormatCompatibilityCheckExpInternal = find_symbol<decltype(zeDriverRTASFormatCompatibilityCheckExp)*>(handle,"zeDriverRTASFormatCompatibilityCheckExp");
  zeRTASBuilderGetBuildPropertiesExpInternal = find_symbol<decltype(zeRTASBuilderGetBuildPropertiesExp)*>(handle,"zeRTASBuilderGetBuildPropertiesExp");
  zeRTASBuilderBuildExpInternal = find_symbol<decltype(zeRTASBuilderBuildExp)*>(handle,"zeRTASBuilderBuildExp");
  
  zeRTASParallelOperationCreateExpInternal = find_symbol<decltype(zeRTASParallelOperationCreateExp)*>(handle,"zeRTASParallelOperationCreateExp");
  zeRTASParallelOperationDestroyExpInternal = find_symbol<decltype(zeRTASParallelOperationDestroyExp)*>(handle,"zeRTASParallelOperationDestroyExp");
  zeRTASParallelOperationGetPropertiesExpInternal = find_symbol<decltype(zeRTASParallelOperationGetPropertiesExp)*>(handle,"zeRTASParallelOperationGetPropertiesExp");
  zeRTASParallelOperationJoinExpInternal = find_symbol<decltype(zeRTASParallelOperationJoinExp)*>(handle,"zeRTASParallelOperationJoinExp");

  ZeWrapper::rtas_builder = ZeWrapper::LEVEL_ZERO;
  return ZE_RESULT_SUCCESS;
}

ze_result_t selectLevelZeroRTASBuilderExt(ze_driver_handle_t hDriver)
{
  if (ZeWrapper::rtas_builder == ZeWrapper::LEVEL_ZERO)
    return ZE_RESULT_SUCCESS;

  auto zeRTASBuilderCreateExtTemp = find_symbol<decltype(zeRTASBuilderCreateExt)*>(handle,"zeRTASBuilderCreateExt");
  auto zeRTASBuilderDestroyExtTemp = find_symbol<decltype(zeRTASBuilderDestroyExt)*>(handle,"zeRTASBuilderDestroyExt");
  
  ze_rtas_builder_ext_desc_t builderDesc = { ZE_STRUCTURE_TYPE_RTAS_BUILDER_EXT_DESC };
  ze_rtas_builder_ext_handle_t hBuilder = nullptr;
  ze_result_t err = zeRTASBuilderCreateExtTemp(hDriver, &builderDesc, &hBuilder);

  /* when ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE is reported extension cannot get loaded */
  if (err == ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE)
    return err;

  if (err == ZE_RESULT_SUCCESS)
    zeRTASBuilderDestroyExtTemp(hBuilder);

  zeRTASBuilderCreateExtInternal = zeRTASBuilderCreateExtTemp;
  zeRTASBuilderDestroyExtInternal = zeRTASBuilderDestroyExtTemp;
  
  zeDriverRTASFormatCompatibilityCheckExtInternal = find_symbol<decltype(zeDriverRTASFormatCompatibilityCheckExt)*>(handle,"zeDriverRTASFormatCompatibilityCheckExt");
  zeRTASBuilderGetBuildPropertiesExtInternal = find_symbol<decltype(zeRTASBuilderGetBuildPropertiesExt)*>(handle,"zeRTASBuilderGetBuildPropertiesExt");
  zeRTASBuilderBuildExtInternal = find_symbol<decltype(zeRTASBuilderBuildExt)*>(handle,"zeRTASBuilderBuildExt");
  zeRTASBuilderCommandListAppendCopyExtInternal = find_symbol<decltype(zeRTASBuilderCommandListAppendCopyExt)*>(handle,"zeRTASBuilderCommandListAppendCopyExt");
  
  zeRTASParallelOperationCreateExtInternal = find_symbol<decltype(zeRTASParallelOperationCreateExt)*>(handle,"zeRTASParallelOperationCreateExt");
  zeRTASParallelOperationDestroyExtInternal = find_symbol<decltype(zeRTASParallelOperationDestroyExt)*>(handle,"zeRTASParallelOperationDestroyExt");
  zeRTASParallelOperationGetPropertiesExtInternal = find_symbol<decltype(zeRTASParallelOperationGetPropertiesExt)*>(handle,"zeRTASParallelOperationGetPropertiesExt");
  zeRTASParallelOperationJoinExtInternal = find_symbol<decltype(zeRTASParallelOperationJoinExt)*>(handle,"zeRTASParallelOperationJoinExt");

  ZeWrapper::rtas_builder = ZeWrapper::LEVEL_ZERO;
  return ZE_RESULT_SUCCESS;
}

ze_result_t selectLevelZeroRTASBuilder(API_TY aty, ze_driver_handle_t hDriver)
{
  switch (aty) {
  case EXP_API: return selectLevelZeroRTASBuilderExp(hDriver);
  case EXT_API: return selectLevelZeroRTASBuilderExt(hDriver);
  default: assert(false); return ZE_RESULT_ERROR_UNKNOWN;
  }
}

ze_result_t selectInternalRTASBuilder()
{
#if defined(ZE_RAYTRACING_DISABLE_INTERNAL_BUILDER)
  throw std::runtime_error("internal builder disabled at compile time");
#else
  if (ZeWrapper::rtas_builder == ZeWrapper::INTERNAL)
    return ZE_RESULT_SUCCESS;
  
  zeRTASBuilderCreateExpInternal = &zeRTASBuilderCreateExpImpl;
  zeRTASBuilderDestroyExpInternal = &zeRTASBuilderDestroyExpImpl;
  zeDriverRTASFormatCompatibilityCheckExpInternal = &zeDriverRTASFormatCompatibilityCheckExpImpl;
  zeRTASBuilderGetBuildPropertiesExpInternal = &zeRTASBuilderGetBuildPropertiesExpImpl;
  zeRTASBuilderBuildExpInternal = &zeRTASBuilderBuildExpImpl;
  
  zeRTASParallelOperationCreateExpInternal = &zeRTASParallelOperationCreateExpImpl;
  zeRTASParallelOperationDestroyExpInternal = &zeRTASParallelOperationDestroyExpImpl;
  zeRTASParallelOperationGetPropertiesExpInternal = &zeRTASParallelOperationGetPropertiesExpImpl;
  zeRTASParallelOperationJoinExpInternal = &zeRTASParallelOperationJoinExpImpl;

  zeRTASBuilderCreateExtInternal = &zeRTASBuilderCreateExtImpl;
  zeRTASBuilderDestroyExtInternal = &zeRTASBuilderDestroyExtImpl;
  zeDriverRTASFormatCompatibilityCheckExtInternal = &zeDriverRTASFormatCompatibilityCheckExtImpl;
  zeRTASBuilderGetBuildPropertiesExtInternal = &zeRTASBuilderGetBuildPropertiesExtImpl;
  zeRTASBuilderBuildExtInternal = &zeRTASBuilderBuildExtImpl;
  
  zeRTASParallelOperationCreateExtInternal = &zeRTASParallelOperationCreateExtImpl;
  zeRTASParallelOperationDestroyExtInternal = &zeRTASParallelOperationDestroyExtImpl;
  zeRTASParallelOperationGetPropertiesExtInternal = &zeRTASParallelOperationGetPropertiesExtImpl;
  zeRTASParallelOperationJoinExtInternal = &zeRTASParallelOperationJoinExtImpl;

  ZeWrapper::rtas_builder = ZeWrapper::INTERNAL;
#endif
  return ZE_RESULT_SUCCESS;
}

ze_result_t ZeWrapper::init()
{
  std::lock_guard<std::mutex> lock(zeWrapperMutex);
  if (handle)
    return ZE_RESULT_SUCCESS;

  try {
    handle = load_module();
    
    zeMemFreeInternal = find_symbol<decltype(zeMemFree)*>(handle, "zeMemFree");
    zeMemAllocSharedInternal = find_symbol<decltype(zeMemAllocShared)*>(handle, "zeMemAllocShared");
    zeMemAllocDeviceInternal = find_symbol<decltype(zeMemAllocDevice)*>(handle, "zeMemAllocDevice");
    zeDriverGetExtensionPropertiesInternal = find_symbol<decltype(zeDriverGetExtensionProperties)*>(handle, "zeDriverGetExtensionProperties");
    zeDeviceGetPropertiesInternal = find_symbol<decltype(zeDeviceGetProperties)*>(handle, "zeDeviceGetProperties");
    zeDeviceGetModulePropertiesInternal = find_symbol<decltype(zeDeviceGetModuleProperties)*>(handle, "zeDeviceGetModuleProperties");
    zeCommandListAppendMemoryCopyInternal = find_symbol<decltype(zeCommandListAppendMemoryCopy)*>(handle, "zeCommandListAppendMemoryCopy");
    zeCommandQueueExecuteCommandListsInternal = find_symbol<decltype(zeCommandQueueExecuteCommandLists)*>(handle, "zeCommandQueueExecuteCommandLists");
    zeCommandListCreateInternal = find_symbol<decltype(zeCommandListCreate)*>(handle, "zeCommandListCreate");
    zeCommandListCloseInternal = find_symbol<decltype(zeCommandListClose)*>(handle, "zeCommandListClose");
    zeCommandListDestroyInternal = find_symbol<decltype(zeCommandListDestroy)*>(handle, "zeCommandListDestroy");
  }
  catch (std::exception& e) {
    return ZE_RESULT_ERROR_UNKNOWN;
  }
  return ZE_RESULT_SUCCESS;
}

ze_result_t ZeWrapper::initRTASBuilder(API_TY aty, ze_driver_handle_t hDriver, RTAS_BUILD_MODE rtas_build_mode)
{
  std::lock_guard<std::mutex> lock(zeWrapperMutex);

  /* only select rtas builder once! */
  if (rtas_builder == rtas_build_mode)
    return ZE_RESULT_SUCCESS;

  try {
    
    if (rtas_build_mode == RTAS_BUILD_MODE::INTERNAL)
      return selectInternalRTASBuilder();
    
    else if (rtas_build_mode == RTAS_BUILD_MODE::LEVEL_ZERO)
      return selectLevelZeroRTASBuilder(aty,hDriver);
    
    else
      throw std::runtime_error("internal error");
  }
  catch (std::exception& e) {
    return ZE_RESULT_ERROR_UNKNOWN;
  }
  return ZE_RESULT_ERROR_UNKNOWN;
}

ze_result_t ZeWrapper::zeMemFree(ze_context_handle_t context, void* ptr)
{
  if (!handle || !zeMemFreeInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeMemFreeInternal(context, ptr);
}

ze_result_t ZeWrapper::zeMemAllocShared(ze_context_handle_t context, const ze_device_mem_alloc_desc_t* descd, const ze_host_mem_alloc_desc_t* desch, size_t s0, size_t s1, ze_device_handle_t ze_handle, void** ptr)
{
  if (!handle || !zeMemAllocSharedInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeMemAllocSharedInternal(context, descd, desch, s0, s1, ze_handle, ptr);
}

ze_result_t ZeWrapper::zeMemAllocDevice(ze_context_handle_t context, const ze_device_mem_alloc_desc_t* descd, size_t s0, size_t s1, ze_device_handle_t ze_handle, void** ptr)
{
  if (!handle || !zeMemAllocDeviceInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeMemAllocDeviceInternal(context, descd, s0, s1, ze_handle, ptr);
}

ze_result_t ZeWrapper::zeDriverGetExtensionProperties(ze_driver_handle_t ze_handle, uint32_t* ptr, ze_driver_extension_properties_t* props)
{
  if (!handle || !zeDriverGetExtensionPropertiesInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeDriverGetExtensionPropertiesInternal(ze_handle, ptr, props);
}

#define VALIDATE(arg) \
  {\
  ze_result_t result = validate(arg);\
  if (result != ZE_RESULT_SUCCESS) return result; \
  }

ze_result_t validate(ze_device_handle_t hDevice)
  {
    if (hDevice == nullptr)
      return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    
    return ZE_RESULT_SUCCESS;
  }

ze_result_t validate(ze_rtas_device_exp_properties_t* pProperties)
{ 
  if (pProperties == nullptr)
    return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
  
  //if (pProperties->stype != ZE_STRUCTURE_TYPE_RTAS_DEVICE_EXP_PROPERTIES)
  //  return ZE_RESULT_ERROR_INVALID_ENUMERATION;
  
  //if (!checkDescChain((zet_base_desc_t_*)pProperties))
  //return ZE_RESULT_ERROR_INVALID_ENUMERATION;
  
  return ZE_RESULT_SUCCESS;
}

ze_result_t zeDeviceGetRTASPropertiesExp( const ze_device_handle_t hDevice, ze_rtas_device_exp_properties_t* pProperties )
{
  /* input validation */
  VALIDATE(hDevice);
  VALIDATE(pProperties);
  
  /* fill properties */
  pProperties->flags = 0;
  pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_INVALID;
  pProperties->rtasBufferAlignment = 128;

  /* check for supported device ID */
  ze_device_properties_t device_props{ ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES };
  ze_result_t status = ZeWrapper::zeDeviceGetProperties(hDevice, &device_props);
  if (status != ZE_RESULT_SUCCESS)
    return status;
  
  /* check for Intel vendor */
  const uint32_t vendor_id = device_props.vendorId;
  const uint32_t device_id = device_props.deviceId;
  if (vendor_id != 0x8086) return ZE_RESULT_ERROR_UNKNOWN;
  
  /* disabling of device check through env variable */
  const char* disable_device_check = getenv("EMBREE_DISABLE_DEVICEID_CHECK");
  if (disable_device_check && strcmp(disable_device_check,"1") == 0) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }
  
  /* DG2 */
  const bool dg2 =
    (0x4F80 <= device_id && device_id <= 0x4F88) ||
    (0x5690 <= device_id && device_id <= 0x5698) ||
    (0x56A0 <= device_id && device_id <= 0x56A6) ||
    (0x56B0 <= device_id && device_id <= 0x56B3) ||
    (0x56C0 <= device_id && device_id <= 0x56C1);
  
  if (dg2) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }

  /* BMG */
  const bool bmg =
    (device_id == 0xE202) ||
    (device_id == 0xE20B) ||
    (device_id == 0xE20C) ||
    (device_id == 0xE20D) ||
    (device_id == 0xE210) ||
    (device_id == 0xE212) ||
    (device_id == 0xE216);
  
  if (bmg) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }
  
  /* PVC */
  const bool pvc =
    (0x0BD5 <= device_id && device_id <= 0x0BDB) ||
    (device_id == 0x0B69) ||
    (device_id == 0x0B6E) ||
    (device_id == 0x0BD4);
  
  if (pvc) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }
  
  /* MTL */
  const bool mtl =
    (device_id == 0x7D40) ||
    (device_id == 0x7D55) ||
    (device_id == 0x7DD5) ||
    (device_id == 0x7D45) ||
    (device_id == 0x7D60);
  
  if (mtl) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }

  /* LNL */
  bool lnl = 
    device_id == 0x64A0 ||
    device_id == 0x6420;
  
  if (lnl) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_1;
    return ZE_RESULT_SUCCESS;
  }
  
  /* PTL */
  bool ptl =
    device_id == 0xB080 ||
    device_id == 0xB081 ||
    device_id == 0xB082 ||
    device_id == 0xB083 ||
    device_id == 0xB08F ||
    device_id == 0xB090 ||
    device_id == 0xB0A0 ||
    device_id == 0xB0B0;
  
  if (ptl) {
    pProperties->rtasFormat = (ze_rtas_format_exp_t) ZE_RTAS_DEVICE_FORMAT_EXP_VERSION_2;
    return ZE_RESULT_SUCCESS;
  }
  
  return ZE_RESULT_ERROR_UNKNOWN;
}

ze_result_t ZeWrapper::zeDeviceGetProperties(ze_device_handle_t ze_handle, ze_device_properties_t* props)
{
  if (!handle || !zeDeviceGetPropertiesInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  if (ZeWrapper::rtas_builder == ZeWrapper::INTERNAL)
  {
    if (props->pNext && (((ze_base_properties_t*)props->pNext)->stype == ZE_STRUCTURE_TYPE_RTAS_DEVICE_EXP_PROPERTIES ||
                         ((ze_base_properties_t*)props->pNext)->stype == ZE_STRUCTURE_TYPE_RTAS_DEVICE_EXT_PROPERTIES))
    {
      ze_result_t result = zeDeviceGetRTASPropertiesExp(ze_handle, (ze_rtas_device_exp_properties_t*)props->pNext);
      if (result != ZE_RESULT_SUCCESS) return result;
      
      void* pNext = props->pNext;
      props->pNext = ((ze_base_properties_t*)props->pNext)->pNext;
      result = zeDeviceGetPropertiesInternal(ze_handle, props);
      props->pNext = pNext;
      return result;
    }
  }
    
  return zeDeviceGetPropertiesInternal(ze_handle, props);
}

ze_result_t ZeWrapper::zeDeviceGetModuleProperties(ze_device_handle_t ze_handle, ze_device_module_properties_t* props)
{
  if (!handle || !zeDeviceGetModulePropertiesInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeDeviceGetModulePropertiesInternal(ze_handle, props);
}

/* EXP version of API */

ze_result_t ZeWrapper::zeRTASBuilderCreateExp(ze_driver_handle_t hDriver, const ze_rtas_builder_exp_desc_t *pDescriptor, ze_rtas_builder_exp_handle_t *phBuilder)
{
  if (!handle || !zeRTASBuilderCreateExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASBuilderCreateExpInternal(hDriver,pDescriptor,phBuilder);
}

ze_result_t ZeWrapper::zeRTASBuilderDestroyExp(ze_rtas_builder_exp_handle_t hBuilder)
{
  if (!handle || !zeRTASBuilderDestroyExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
    
  return zeRTASBuilderDestroyExpInternal(hBuilder);
}

ze_result_t ZeWrapper::zeDriverRTASFormatCompatibilityCheckExp( ze_driver_handle_t hDriver,
                                                                 const ze_rtas_format_exp_t accelFormat,
                                                                 const ze_rtas_format_exp_t otherAccelFormat)
{
  if (!handle || !zeDriverRTASFormatCompatibilityCheckExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeDriverRTASFormatCompatibilityCheckExpInternal( hDriver, accelFormat, otherAccelFormat);
}

ze_result_t ZeWrapper::zeRTASBuilderGetBuildPropertiesExp(ze_rtas_builder_exp_handle_t hBuilder,
                                                          const ze_rtas_builder_build_op_exp_desc_t* args,
                                                          ze_rtas_builder_exp_properties_t* pProp)
{
  if (!handle || !zeRTASBuilderGetBuildPropertiesExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
    
  return zeRTASBuilderGetBuildPropertiesExpInternal(hBuilder, args, pProp);
}
  
ze_result_t ZeWrapper::zeRTASBuilderBuildExp(ze_rtas_builder_exp_handle_t hBuilder,
                                             const ze_rtas_builder_build_op_exp_desc_t* args,
                                             void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                             void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                             ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                                             void *pBuildUserPtr, ze_rtas_aabb_exp_t *pBounds, size_t *pRtasBufferSizeBytes)
{
  if (!handle || !zeRTASBuilderBuildExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASBuilderBuildExpInternal(hBuilder, args, pScratchBuffer, scratchBufferSizeBytes, pRtasBuffer, rtasBufferSizeBytes,
                                       hParallelOperation, pBuildUserPtr, pBounds, pRtasBufferSizeBytes);
}

ze_result_t ZeWrapper::zeRTASBuilderCommandListAppendCopyExp(ze_command_list_handle_t hCommandList,
                                                             void* dstptr,
                                                             const void* srcptr,
                                                             size_t size,
                                                             ze_event_handle_t hSignalEvent,
                                                             uint32_t numWaitEvents,
                                                             ze_event_handle_t* phWaitEvents)
{
  if (!handle || !zeCommandListAppendMemoryCopyInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeCommandListAppendMemoryCopyInternal(hCommandList,dstptr,srcptr,size,hSignalEvent,numWaitEvents,phWaitEvents); // EXP API does not have proper copy function
}

ze_result_t ZeWrapper::zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue, uint32_t numCommandLists, ze_command_list_handle_t* phCommandLists, ze_fence_handle_t hFence)
{
  if (!handle || !zeCommandQueueExecuteCommandListsInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeCommandQueueExecuteCommandListsInternal(hCommandQueue, numCommandLists, phCommandLists, hFence);
}

ze_result_t ZeWrapper::zeCommandListCreate(ze_context_handle_t hContext, ze_device_handle_t hDevice, const ze_command_list_desc_t* desc, ze_command_list_handle_t* phCommandList)
{
  if (!handle || !zeCommandListCreateInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeCommandListCreateInternal(hContext, hDevice, desc, phCommandList);
}

ze_result_t ZeWrapper::zeCommandListClose(ze_command_list_handle_t hCommandList)
{
  if (!handle || !zeCommandListCloseInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeCommandListCloseInternal(hCommandList);
}

ze_result_t ZeWrapper::zeCommandListDestroy(ze_command_list_handle_t hCommandList)
{
  if (!handle || !zeCommandListDestroyInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeCommandListDestroyInternal(hCommandList);
}

ze_result_t ZeWrapper::zeRTASParallelOperationCreateExp(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_exp_handle_t* phParallelOperation)
{
  if (!handle || !zeRTASParallelOperationCreateExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeRTASParallelOperationCreateExpInternal(hDriver, phParallelOperation);
}

ze_result_t ZeWrapper::zeRTASParallelOperationDestroyExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation )
{
  if (!handle || !zeRTASParallelOperationDestroyExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationDestroyExpInternal( hParallelOperation );
};

ze_result_t ZeWrapper::zeRTASParallelOperationGetPropertiesExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation, ze_rtas_parallel_operation_exp_properties_t* pProperties )
{
  if (!handle || !zeRTASParallelOperationGetPropertiesExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationGetPropertiesExpInternal( hParallelOperation, pProperties );
}
 
ze_result_t ZeWrapper::zeRTASParallelOperationJoinExp( ze_rtas_parallel_operation_exp_handle_t hParallelOperation)
{
  if (!handle || !zeRTASParallelOperationJoinExpInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationJoinExpInternal(hParallelOperation);
}


/* EXT version of API */

ze_result_t ZeWrapper::zeRTASBuilderCreateExt(ze_driver_handle_t hDriver, const ze_rtas_builder_ext_desc_t *pDescriptor, ze_rtas_builder_ext_handle_t *phBuilder)
{
  if (!handle || !zeRTASBuilderCreateExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASBuilderCreateExtInternal(hDriver,pDescriptor,phBuilder);
}

ze_result_t ZeWrapper::zeRTASBuilderDestroyExt(ze_rtas_builder_ext_handle_t hBuilder)
{
  if (!handle || !zeRTASBuilderDestroyExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
    
  return zeRTASBuilderDestroyExtInternal(hBuilder);
}

ze_result_t ZeWrapper::zeDriverRTASFormatCompatibilityCheckExt( ze_driver_handle_t hDriver,
                                                                 const ze_rtas_format_ext_t accelFormat,
                                                                 const ze_rtas_format_ext_t otherAccelFormat)
{
  if (!handle || !zeDriverRTASFormatCompatibilityCheckExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeDriverRTASFormatCompatibilityCheckExtInternal( hDriver, accelFormat, otherAccelFormat);
}

ze_result_t ZeWrapper::zeRTASBuilderGetBuildPropertiesExt(ze_rtas_builder_ext_handle_t hBuilder,
                                                          const ze_rtas_builder_build_op_ext_desc_t* args,
                                                          ze_rtas_builder_ext_properties_t* pProp)
{
  if (!handle || !zeRTASBuilderGetBuildPropertiesExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
    
  return zeRTASBuilderGetBuildPropertiesExtInternal(hBuilder, args, pProp);
}
  
ze_result_t ZeWrapper::zeRTASBuilderBuildExt(ze_rtas_builder_ext_handle_t hBuilder,
                                             const ze_rtas_builder_build_op_ext_desc_t* args,
                                             void *pScratchBuffer, size_t scratchBufferSizeBytes,
                                             void *pRtasBuffer, size_t rtasBufferSizeBytes,
                                             ze_rtas_parallel_operation_ext_handle_t hParallelOperation,
                                             void *pBuildUserPtr, ze_rtas_aabb_ext_t *pBounds, size_t *pRtasBufferSizeBytes)
{
  if (!handle || !zeRTASBuilderBuildExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASBuilderBuildExtInternal(hBuilder, args, pScratchBuffer, scratchBufferSizeBytes, pRtasBuffer, rtasBufferSizeBytes,
                                       hParallelOperation, pBuildUserPtr, pBounds, pRtasBufferSizeBytes);
}

ze_result_t ZeWrapper::zeRTASBuilderCommandListAppendCopyExt(ze_command_list_handle_t hCommandList,
                                                             void* dstptr,
                                                             const void* srcptr,
                                                             size_t size,
                                                             ze_event_handle_t hSignalEvent,
                                                             uint32_t numWaitEvents,
                                                             ze_event_handle_t* phWaitEvents)
{
  if (!handle || !zeRTASBuilderCommandListAppendCopyExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeRTASBuilderCommandListAppendCopyExtInternal(hCommandList,dstptr,srcptr,size,hSignalEvent,numWaitEvents,phWaitEvents);
}

ze_result_t ZeWrapper::zeRTASParallelOperationCreateExt(ze_driver_handle_t hDriver, ze_rtas_parallel_operation_ext_handle_t* phParallelOperation)
{
  if (!handle || !zeRTASParallelOperationCreateExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");

  return zeRTASParallelOperationCreateExtInternal(hDriver, phParallelOperation);
}

ze_result_t ZeWrapper::zeRTASParallelOperationDestroyExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation )
{
  if (!handle || !zeRTASParallelOperationDestroyExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationDestroyExtInternal( hParallelOperation );
};

ze_result_t ZeWrapper::zeRTASParallelOperationGetPropertiesExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation, ze_rtas_parallel_operation_ext_properties_t* pProperties )
{
  if (!handle || !zeRTASParallelOperationGetPropertiesExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationGetPropertiesExtInternal( hParallelOperation, pProperties );
}
 
ze_result_t ZeWrapper::zeRTASParallelOperationJoinExt( ze_rtas_parallel_operation_ext_handle_t hParallelOperation)
{
  if (!handle || !zeRTASParallelOperationJoinExtInternal)
    throw std::runtime_error("ZeWrapper not initialized, call ZeWrapper::init() first.");
  
  return zeRTASParallelOperationJoinExtInternal(hParallelOperation);
}
