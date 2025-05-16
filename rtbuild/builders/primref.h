// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../sys/platform.h"
#include "../sys/sysinfo.h"
#include "../math/emath.h"
#include "../simd/simd.h"
#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/vec3fa.h"
#include "../math/bbox.h"
#include "../math/affinespace.h"
#include "../math/range.h"

namespace embree
{
  /*! A primitive reference stores the bounds of the primitive and its ID. */
  struct __aligned(32) PrimRef 
  {
    __forceinline PrimRef () {}

    __forceinline PrimRef (const BBox3fa& bounds, unsigned int geomID, unsigned int primID) 
    {
      lower = Vec3fx(bounds.lower, geomID);
      upper = Vec3fx(bounds.upper, primID);
    }

    __forceinline PrimRef (const BBox3fa& bounds, size_t id) 
    {
#if defined(__64BIT__)
      lower = Vec3fx(bounds.lower, (unsigned)(id & 0xFFFFFFFF));
      upper = Vec3fx(bounds.upper, (unsigned)((id >> 32) & 0xFFFFFFFF));
#else
      lower = Vec3fx(bounds.lower, (unsigned)id);
      upper = Vec3fx(bounds.upper, (unsigned)0);
#endif
    }

    /*! calculates twice the center of the primitive */
    __forceinline const Vec3fa center2() const {
      return lower+upper;
    }
    
    /*! return the bounding box of the primitive */
    __forceinline const BBox3fa bounds() const {
      return BBox3fa(lower,upper);
    }

    /*! size for bin heuristic is 1 */
    __forceinline unsigned size() const { 
      return 1;
    }

    /*! returns bounds and centroid used for binning */
    __forceinline void binBoundsAndCenter(BBox3fa& bounds_o, Vec3fa& center_o) const 
    {
      bounds_o = bounds();
      center_o = embree::center2(bounds_o);
    }

    /*! returns the geometry ID */
    __forceinline unsigned geomID() const { 
      return lower.a;
    }

    /*! returns the primitive ID */
    __forceinline unsigned primID() const { 
      return upper.a;
    }

    /*! returns an size_t sized ID */
    __forceinline size_t ID() const { 
#if defined(__64BIT__)
      return size_t(lower.u) + (size_t(upper.u) << 32);
#else
      return size_t(lower.u);
#endif
    }

    /*! special function for operator< */
    __forceinline uint64_t ID64() const {
      return (((uint64_t)primID()) << 32) + (uint64_t)geomID();
    }
    
    /*! allows sorting the primrefs by ID */
    friend __forceinline bool operator<(const PrimRef& p0, const PrimRef& p1) {
      return p0.ID64() < p1.ID64();
    }

    /*! Outputs primitive reference to a stream. */
    friend __forceinline embree_ostream operator<<(embree_ostream cout, const PrimRef& ref) {
      return cout << "{ lower = " << ref.lower << ", upper = " << ref.upper << ", geomID = " << ref.geomID() << ", primID = " << ref.primID() << " }";
    }

  public:
    Vec3fx lower;     //!< lower bounds and geomID
    Vec3fx upper;     //!< upper bounds and primID
  };

  /*! fast exchange for PrimRefs */
  __forceinline void xchg(PrimRef& a, PrimRef& b)
  {
    std::swap(a,b);
  }
}
