// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "vec3.h"

namespace embree
{
  ////////////////////////////////////////////////////////////////////////////////
  /// Generic 4D vector Class
  ////////////////////////////////////////////////////////////////////////////////

  template<typename T> struct Vec4
  {
    T x, y, z, w;

    __forceinline Vec4( ) {}
    __forceinline explicit Vec4( const T& a                                     ) : x(a), y(a), z(a), w(a) {}
    __forceinline          Vec4( const T& x, const T& y, const T& z, const T& w ) : x(x), y(y), z(z), w(w) {}
    __forceinline          Vec4( const Vec3<T>& v, const T& w ) : x(v.x), y(v.y), z(v.z), w(w) {}

    __forceinline Vec3<T> xyz() const { return Vec3<T>(x,y,z); }
    
    ////////////////////////////////////////////////////////////////////////////////
    /// Constants
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline Vec4( ZeroTy   ) : x(zero), y(zero), z(zero), w(zero) {}
    __forceinline Vec4( OneTy    ) : x(one),  y(one),  z(one),  w(one) {}

    __forceinline const T& operator []( const size_t axis ) const { assert(axis < 4); return (&x)[axis]; }
    __forceinline       T& operator []( const size_t axis )       { assert(axis < 4); return (&x)[axis]; }
  };

  template<typename T>
  __forceinline Vec4<T> rotate_right(Vec4<T> v, uint32_t r = 1) {
    r = r%4;
    return Vec4<T>( v[(4-r)%4], v[(5-r)%4], v[(6-r)%4], v[(7-r)%4] );
  }
  
  template<typename T>
  __forceinline Vec4<T> rotate_left(Vec4<T> v, uint32_t r = 1) {
    r = r%4;
    return Vec4<T>(v[(0+r)%4], v[(1+r)%4], v[(2+r)%4], v[(3+r)%4]);
  }
  
  ////////////////////////////////////////////////////////////////////////////////
  /// Reduction Operators
  ////////////////////////////////////////////////////////////////////////////////

  template<typename T> __forceinline T reduce_add( const Vec4<T>& a ) { return a.x + a.y + a.z + a.w; }
  template<typename T> __forceinline T reduce_mul( const Vec4<T>& a ) { return a.x * a.y * a.z * a.w; }
  template<typename T> __forceinline T reduce_min( const Vec4<T>& a ) { return min(a.x, min(a.y, min(a.z, a.w))); }
  template<typename T> __forceinline T reduce_max( const Vec4<T>& a ) { return max(a.x, max(a.y, max(a.z, a.w))); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Comparison Operators
  ////////////////////////////////////////////////////////////////////////////////

  template<typename T> __forceinline bool operator ==( const Vec4<T>& a, const Vec4<T>& b ) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
  template<typename T> __forceinline bool operator !=( const Vec4<T>& a, const Vec4<T>& b ) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Output Operators
  ////////////////////////////////////////////////////////////////////////////////

  template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Vec4<T>& a) {
    return cout << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
  }

  typedef Vec4<bool > Vec4b;
  typedef Vec4<int  > Vec4i;
  typedef Vec4<float> Vec4f;
}
