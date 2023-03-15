// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../sys/array.h"
#include "../math/emath.h"
#include "../math/range.h"

// We need to define these to avoid implicit linkage against
// tbb_debug.lib under Windows. When removing these lines debug build
// under Windows fails.
#define __TBB_NO_IMPLICIT_LINKAGE 1
#define __TBBMALLOC_NO_IMPLICIT_LINKAGE 1
#define TBB_SUPPRESS_DEPRECATED_MESSAGES 1
#define TBB_PREVIEW_ISOLATED_TASK_GROUP 1
#include "tbb/tbb.h"

namespace embree
{
  struct TaskScheduler
  {
    /* returns the total number of threads */
    static __forceinline size_t threadCount() {
#if TBB_INTERFACE_VERSION >= 9100
      return tbb::this_task_arena::max_concurrency();
#else
      return tbb::task_scheduler_init::default_num_threads();
#endif
    }
  };
  
  /* parallel_for without range */
  template<typename Index, typename Func>
    __forceinline void parallel_for( const Index N, const Func& func)
  {
  #if TBB_INTERFACE_VERSION >= 12002
    tbb::task_group_context context;
    tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
        func(i);
      },context);
    if (context.is_group_execution_cancelled())
      throw std::runtime_error("task cancelled");
  #else
    tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
        func(i);
      });
    if (tbb::task::self().is_cancelled())
      throw std::runtime_error("task cancelled");
  #endif
  }
  
  /* parallel for with range and granulatity */
  template<typename Index, typename Func>
    __forceinline void parallel_for( const Index first, const Index last, const Index minStepSize, const Func& func)
  {
    assert(first <= last);

  #if TBB_INTERFACE_VERSION >= 12002
    tbb::task_group_context context;
    tbb::parallel_for(tbb::blocked_range<Index>(first,last,minStepSize),[&](const tbb::blocked_range<Index>& r) {
        func(range<Index>(r.begin(),r.end()));
      },context);
    if (context.is_group_execution_cancelled())
      throw std::runtime_error("task cancelled");
  #else
    tbb::parallel_for(tbb::blocked_range<Index>(first,last,minStepSize),[&](const tbb::blocked_range<Index>& r) {
        func(range<Index>(r.begin(),r.end()));
      });
    if (tbb::task::self().is_cancelled())
      throw std::runtime_error("task cancelled");
  #endif

  }
  
  /* parallel for with range */
  template<typename Index, typename Func>
    __forceinline void parallel_for( const Index first, const Index last, const Func& func)
  {
    assert(first <= last);
    parallel_for(first,last,(Index)1,func);
  }

#if (TBB_INTERFACE_VERSION > 4001)

  template<typename Index, typename Func>
    __forceinline void parallel_for_static( const Index N, const Func& func)
  {
    #if TBB_INTERFACE_VERSION >= 12002
      tbb::task_group_context context;
      tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
          func(i);
        },tbb::simple_partitioner(),context);
      if (context.is_group_execution_cancelled())
        throw std::runtime_error("task cancelled");
    #else
      tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
          func(i);
        },tbb::simple_partitioner());
      if (tbb::task::self().is_cancelled())
        throw std::runtime_error("task cancelled");
    #endif
  }

  typedef tbb::affinity_partitioner affinity_partitioner;

  template<typename Index, typename Func>
    __forceinline void parallel_for_affinity( const Index N, const Func& func, tbb::affinity_partitioner& ap)
  {
    #if TBB_INTERFACE_VERSION >= 12002
      tbb::task_group_context context;
      tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
          func(i);
        },ap,context);
      if (context.is_group_execution_cancelled())
        throw std::runtime_error("task cancelled");
    #else
      tbb::parallel_for(Index(0),N,Index(1),[&](Index i) {
          func(i);
        },ap);
      if (tbb::task::self().is_cancelled())
        throw std::runtime_error("task cancelled");
    #endif
  }

#else

  template<typename Index, typename Func>
    __forceinline void parallel_for_static( const Index N, const Func& func) 
  {
    parallel_for(N,func);
  }

  struct affinity_partitioner {
  };

  template<typename Index, typename Func>
    __forceinline void parallel_for_affinity( const Index N, const Func& func, affinity_partitioner& ap) 
  {
    parallel_for(N,func);
  }

#endif
}
