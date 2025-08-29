// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "alloc.h"
#include "intrinsics.h"
#include "sysinfo.h"

////////////////////////////////////////////////////////////////////////////////
/// All Platforms
////////////////////////////////////////////////////////////////////////////////
  
namespace embree
{
  size_t total_allocations = 0;

#if defined(EMBREE_SYCL_SUPPORT)
  
  __thread sycl::context* tls_context_tutorial = nullptr;
  __thread sycl::device* tls_device_tutorial = nullptr;
  
  __thread sycl::context* tls_context_embree = nullptr;
  __thread sycl::device* tls_device_embree = nullptr;
  
  void enableUSMAllocEmbree(sycl::context* context, sycl::device* device)
  {
    if (tls_context_embree != nullptr) throw std::runtime_error("USM allocation already enabled");
    if (tls_device_embree != nullptr) throw std::runtime_error("USM allocation already enabled");
    tls_context_embree = context;
    tls_device_embree = device;
  }

  void disableUSMAllocEmbree()
  {
    if (tls_context_embree  == nullptr) throw std::runtime_error("USM allocation not enabled");
    if (tls_device_embree  == nullptr) throw std::runtime_error("USM allocation not enabled");
    tls_context_embree = nullptr;
    tls_device_embree = nullptr;
  }

  void enableUSMAllocTutorial(sycl::context* context, sycl::device* device)
  {
    //if (tls_context_tutorial != nullptr) throw std::runtime_error("USM allocation already enabled");
    //if (tls_device_tutorial != nullptr) throw std::runtime_error("USM allocation already enabled");
    tls_context_tutorial = context;
    tls_device_tutorial = device;
  }

  void disableUSMAllocTutorial()
  {
    if (tls_context_tutorial  == nullptr) throw std::runtime_error("USM allocation not enabled");
    if (tls_device_tutorial  == nullptr) throw std::runtime_error("USM allocation not enabled");
    
    tls_context_tutorial = nullptr;
    tls_device_tutorial = nullptr;
  }

#endif
  
  void* alignedMalloc(size_t size, size_t align)
  {
    if (size == 0)
      return nullptr;

    assert((align & (align-1)) == 0);
    void* ptr = _mm_malloc(size,align);
    if (size != 0 && ptr == nullptr)
      throw std::bad_alloc();
    return ptr;
  }

  void alignedFree(void* ptr)
  {
    if (ptr)
      _mm_free(ptr);
  }

#if defined(EMBREE_SYCL_SUPPORT)
  
  void* alignedSYCLMalloc(sycl::context* context, sycl::device* device, size_t size, size_t align, EmbreeUSMMode mode)
  {
    assert(context);
    assert(device);
    
    if (size == 0)
      return nullptr;

    assert((align & (align-1)) == 0);
    total_allocations++;    

    void* ptr = nullptr;
    if (mode == EMBREE_USM_SHARED_DEVICE_READ_ONLY)
      ptr = sycl::aligned_alloc_shared(align,size,*device,*context,sycl::ext::oneapi::property::usm::device_read_only());
    else
      ptr = sycl::aligned_alloc_shared(align,size,*device,*context);
      
    if (size != 0 && ptr == nullptr)
      throw std::bad_alloc();

    return ptr;
  }
  
  void* alignedSYCLMalloc(size_t size, size_t align, EmbreeUSMMode mode)
  {
    if (tls_context_tutorial) return alignedSYCLMalloc(tls_context_tutorial, tls_device_tutorial, size, align, mode);
    if (tls_context_embree  ) return alignedSYCLMalloc(tls_context_embree,   tls_device_embree,   size, align, mode);
    return nullptr;
  }

  void alignedSYCLFree(sycl::context* context, void* ptr)
  {
    assert(context);
    if (ptr) {
      sycl::free(ptr,*context);
    }
  }

  void alignedSYCLFree(void* ptr)
  {
    if (tls_context_tutorial) return alignedSYCLFree(tls_context_tutorial, ptr);
    if (tls_context_embree  ) return alignedSYCLFree(tls_context_embree, ptr);
  }

#endif

  void* alignedUSMMalloc(size_t size, size_t align, EmbreeUSMMode mode)
  {
#if defined(EMBREE_SYCL_SUPPORT)
    if (tls_context_embree || tls_context_tutorial)
      return alignedSYCLMalloc(size,align,mode);
    else
#endif
      return alignedMalloc(size,align);
  }

  void alignedUSMFree(void* ptr)
  {
#if defined(EMBREE_SYCL_SUPPORT)
    if (tls_context_embree || tls_context_tutorial)
      return alignedSYCLFree(ptr);
    else
#endif
      return alignedFree(ptr);
  }

  static bool huge_pages_enabled = false;

  __forceinline bool isHugePageCandidate(const size_t bytes)
  {
    if (!huge_pages_enabled)
      return false;

    /* use huge pages only when memory overhead is low */
    const size_t hbytes = (bytes+PAGE_SIZE_2M-1) & ~size_t(PAGE_SIZE_2M-1);
    return 66*(hbytes-bytes) < bytes; // at most 1.5% overhead
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>

namespace embree
{
  void* os_malloc(size_t bytes, bool& hugepages)
  {
    if (bytes == 0) {
      hugepages = false;
      return nullptr;
    }

    /* try direct huge page allocation first */
    if (isHugePageCandidate(bytes)) 
    {
      int flags = MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES;
      char* ptr = (char*) VirtualAlloc(nullptr,bytes,flags,PAGE_READWRITE);
      if (ptr != nullptr) {
        hugepages = true;
        return ptr;
      }
    } 

    /* fall back to 4k pages */
    int flags = MEM_COMMIT | MEM_RESERVE;
    char* ptr = (char*) VirtualAlloc(nullptr,bytes,flags,PAGE_READWRITE);
    if (ptr == nullptr) throw std::bad_alloc();
    hugepages = false;
    return ptr;
  }

  size_t os_shrink(void* ptr, size_t bytesNew, size_t bytesOld, bool hugepages) 
  {
    if (hugepages) // decommitting huge pages seems not to work under Windows
      return bytesOld;

    const size_t pageSize = hugepages ? PAGE_SIZE_2M : PAGE_SIZE_4K;
    bytesNew = (bytesNew+pageSize-1) & ~(pageSize-1);
    bytesOld = (bytesOld+pageSize-1) & ~(pageSize-1);
    if (bytesNew >= bytesOld)
      return bytesOld;

    if (!VirtualFree((char*)ptr+bytesNew,bytesOld-bytesNew,MEM_DECOMMIT))
      throw std::bad_alloc();

    return bytesNew;
  }

  void os_free(void* ptr, size_t bytes, bool hugepages) 
  {
    if (bytes == 0) 
      return;

    if (!VirtualFree(ptr,0,MEM_RELEASE))
      throw std::bad_alloc();
  }

  void os_advise(void *ptr, size_t bytes)
  {
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(__UNIX__)

#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#if defined(__MACOSX__)
#include <mach/vm_statistics.h>
#endif

namespace embree
{
  void* os_malloc(size_t bytes, bool& hugepages)
  { 
    if (bytes == 0) {
      hugepages = false;
      return nullptr;
    }

    /* try direct huge page allocation first */
    if (isHugePageCandidate(bytes)) 
    {
#if defined(__MACOSX__)
      void* ptr = mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, VM_FLAGS_SUPERPAGE_SIZE_2MB, 0);
      if (ptr != MAP_FAILED) {
        hugepages = true;
        return ptr;
      }
#elif defined(MAP_HUGETLB)
      void* ptr = mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON | MAP_HUGETLB, -1, 0);
      if (ptr != MAP_FAILED) {
        hugepages = true;
        return ptr;
      }
#endif
    } 

    /* fallback to 4k pages */
    void* ptr = (char*) mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (ptr == MAP_FAILED) throw std::bad_alloc();
    hugepages = false;

    /* advise huge page hint for THP */
    os_advise(ptr,bytes);
    return ptr;
  }

  size_t os_shrink(void* ptr, size_t bytesNew, size_t bytesOld, bool hugepages) 
  {
    const size_t pageSize = hugepages ? PAGE_SIZE_2M : PAGE_SIZE_4K;
    bytesNew = (bytesNew+pageSize-1) & ~(pageSize-1);
    bytesOld = (bytesOld+pageSize-1) & ~(pageSize-1);
    if (bytesNew >= bytesOld)
      return bytesOld;

    if (munmap((char*)ptr+bytesNew,bytesOld-bytesNew) == -1)
      throw std::bad_alloc();

    return bytesNew;
  }

  void os_free(void* ptr, size_t bytes, bool hugepages) 
  {
    if (bytes == 0)
      return;

    /* for hugepages we need to also align the size */
    const size_t pageSize = hugepages ? PAGE_SIZE_2M : PAGE_SIZE_4K;
    bytes = (bytes+pageSize-1) & ~(pageSize-1);
    if (munmap(ptr,bytes) == -1)
      throw std::bad_alloc();
  }

  /* hint for transparent huge pages (THP) */
  void os_advise(void* pptr, size_t bytes)
  {
#if defined(MADV_HUGEPAGE)
    madvise(pptr,bytes,MADV_HUGEPAGE); 
#endif
  }
}

#endif
