// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../math/emath.h"

/* include SSE wrapper classes */
#include "sse.h"

namespace embree
{
  template <int N>
  __forceinline vbool<N> isfinite(const vfloat<N>& v)
  {
    return (v >= vfloat<N>(-std::numeric_limits<float>::max()))
         & (v <= vfloat<N>( std::numeric_limits<float>::max()));
  }
}
