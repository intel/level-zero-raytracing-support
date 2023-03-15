// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../builders/primref.h"

namespace embree
{
  namespace isa
  {
    template<size_t N>
    __forceinline void splitPolygon(const BBox3fa& bounds, 
                                    const size_t dim, 
                                    const float pos, 
                                    const Vec3fa (&v)[N+1],
                                    BBox3fa& left_o, 
                                    BBox3fa& right_o)
    {
      BBox3fa left = empty, right = empty;
      /* clip triangle to left and right box by processing all edges */
      for (size_t i=0; i<N; i++)
      {
        const Vec3fa &v0 = v[i]; 
        const Vec3fa &v1 = v[i+1]; 
        const float v0d = v0[dim];
        const float v1d = v1[dim];
        
        if (v0d <= pos) left. extend(v0); // this point is on left side
        if (v0d >= pos) right.extend(v0); // this point is on right side
        
        if ((v0d < pos && pos < v1d) || (v1d < pos && pos < v0d)) // the edge crosses the splitting location
        {
          assert((v1d-v0d) != 0.0f);
          const float inv_length = 1.0f/(v1d-v0d);
          const Vec3fa c = madd(Vec3fa((pos-v0d)*inv_length),v1-v0,v0);
          left.extend(c);
          right.extend(c);
        }
      }
      
      /* clip against current bounds */
      left_o  = intersect(left,bounds);
      right_o = intersect(right,bounds);
    }
    
    template<size_t N>
    __forceinline void splitPolygon(const BBox3fa& bounds, 
                                    const size_t dim, 
                                    const float pos, 
                                    const Vec3fa (&v)[N+1],
                                    const Vec3fa (&inv_length)[N],
                                    BBox3fa& left_o, 
                                    BBox3fa& right_o)
    {
      BBox3fa left = empty, right = empty;
      /* clip triangle to left and right box by processing all edges */
      for (size_t i=0; i<N; i++)
      {
        const Vec3fa &v0 = v[i]; 
        const Vec3fa &v1 = v[i+1]; 
        const float v0d = v0[dim];
        const float v1d = v1[dim];
        
        if (v0d <= pos) left. extend(v0); // this point is on left side
        if (v0d >= pos) right.extend(v0); // this point is on right side
        
        if ((v0d < pos && pos < v1d) || (v1d < pos && pos < v0d)) // the edge crosses the splitting location
        {
          assert((v1d-v0d) != 0.0f);
          const Vec3fa c = madd(Vec3fa((pos-v0d)*inv_length[i][dim]),v1-v0,v0);
          left.extend(c);
          right.extend(c);
        }
      }
      
      /* clip against current bounds */
      left_o  = intersect(left,bounds);
      right_o = intersect(right,bounds);
    }
    
    template<size_t N>
      __forceinline void splitPolygon(const PrimRef& prim, 
                                      const size_t dim, 
                                      const float pos, 
                                      const Vec3fa (&v)[N+1],
                                      PrimRef& left_o, 
                                      PrimRef& right_o)
    {
      BBox3fa left = empty, right = empty;
      for (size_t i=0; i<N; i++)
      {
        const Vec3fa &v0 = v[i]; 
        const Vec3fa &v1 = v[i+1]; 
        const float v0d = v0[dim];
        const float v1d = v1[dim];
        
        if (v0d <= pos) left. extend(v0); // this point is on left side
        if (v0d >= pos) right.extend(v0); // this point is on right side
        
        if ((v0d < pos && pos < v1d) || (v1d < pos && pos < v0d)) // the edge crosses the splitting location
        {
          assert((v1d-v0d) != 0.0f);
          const float inv_length = 1.0f/(v1d-v0d);
          const Vec3fa c = madd(Vec3fa((pos-v0d)*inv_length),v1-v0,v0);
          left.extend(c);
          right.extend(c);
        }
      }
      
      /* clip against current bounds */
      new (&left_o ) PrimRef(intersect(left ,prim.bounds()),prim.geomID(), prim.primID());
      new (&right_o) PrimRef(intersect(right,prim.bounds()),prim.geomID(), prim.primID());
    }
  }
}

