// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../math/emath.h"

#define vboolf vboolf_impl
#define vboold vboold_impl
#define vint vint_impl
#define vuint vuint_impl
#define vllong vllong_impl
#define vfloat vfloat_impl
#define vdouble vdouble_impl

namespace embree
{
  /* 4-wide SSE integer type */
  template<>
  struct vuint<4>
  {
    ALIGNED_STRUCT_(16);
    
    typedef vboolf4 Bool;
    typedef vuint4   Int;
    typedef vfloat4 Float;

    enum  { size = 4 }; // number of SIMD elements
    union { __m128i v; unsigned int i[4]; }; // data

    ////////////////////////////////////////////////////////////////////////////////
    /// Constructors, Assignment & Cast Operators
    ////////////////////////////////////////////////////////////////////////////////
    
    __forceinline vuint() {}
    __forceinline vuint(const vuint4& a) { v = a.v; }
    __forceinline vuint4& operator =(const vuint4& a) { v = a.v; return *this; }

    __forceinline vuint(const __m128i a) : v(a) {}
    __forceinline operator const __m128i&() const { return v; }
    __forceinline operator       __m128i&()       { return v; }


    __forceinline vuint(unsigned int a) : v(_mm_set1_epi32(a)) {}
    __forceinline vuint(unsigned int a, unsigned int b, unsigned int c, unsigned int d) : v(_mm_set_epi32(d, c, b, a)) {}

    __forceinline explicit vuint(const vboolf4& a) : v(_mm_castps_si128((__m128)a)) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// Constants
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline vuint(ZeroTy)   : v(_mm_setzero_si128()) {}
    __forceinline vuint(OneTy)    : v(_mm_set1_epi32(1)) {}
    __forceinline vuint(PosInfTy) : v(_mm_set1_epi32(unsigned(pos_inf))) {}
    __forceinline vuint(StepTy)   : v(_mm_set_epi32(3, 2, 1, 0)) {}
    __forceinline vuint(TrueTy)   { v = _mm_cmpeq_epi32(v,v); }
    __forceinline vuint(UndefinedTy) : v(_mm_castps_si128(_mm_undefined_ps())) {}

    ////////////////////////////////////////////////////////////////////////////////
    /// Loads and Stores
    ////////////////////////////////////////////////////////////////////////////////

    static __forceinline vuint4 load (const void* a) { return _mm_load_si128((__m128i*)a); }
    static __forceinline vuint4 loadu(const void* a) { return _mm_loadu_si128((__m128i*)a); }

    static __forceinline void store (void* ptr, const vuint4& v) { _mm_store_si128((__m128i*)ptr,v); }
    static __forceinline void storeu(void* ptr, const vuint4& v) { _mm_storeu_si128((__m128i*)ptr,v); }
    
    static __forceinline vuint4 load (const vbool4& mask, const void* a) { return _mm_and_si128(_mm_load_si128 ((__m128i*)a),mask); }
    static __forceinline vuint4 loadu(const vbool4& mask, const void* a) { return _mm_and_si128(_mm_loadu_si128((__m128i*)a),mask); }

    static __forceinline void store (const vboolf4& mask, void* ptr, const vuint4& i) { store (ptr,select(mask,i,load (ptr))); }
    static __forceinline void storeu(const vboolf4& mask, void* ptr, const vuint4& i) { storeu(ptr,select(mask,i,loadu(ptr))); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Array Access
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline const unsigned int& operator [](size_t index) const { assert(index < 4); return i[index]; }
    __forceinline       unsigned int& operator [](size_t index)       { assert(index < 4); return i[index]; }

    friend __forceinline vuint4 select(const vboolf4& m, const vuint4& t, const vuint4& f) {
#if defined(__SSE4_1__)
      return _mm_castps_si128(_mm_blendv_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), m)); 
#else
      return _mm_or_si128(_mm_and_si128(m, t), _mm_andnot_si128(m, f)); 
#endif
    }
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Unary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vboolf4 asBool(const vuint4& a) { return _mm_castsi128_ps(a); }

  __forceinline vuint4 operator +(const vuint4& a) { return a; }
  __forceinline vuint4 operator -(const vuint4& a) { return _mm_sub_epi32(_mm_setzero_si128(), a); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Binary Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vuint4 operator +(const vuint4& a, const vuint4& b) { return _mm_add_epi32(a, b); }
  __forceinline vuint4 operator +(const vuint4& a, unsigned int  b) { return a + vuint4(b); }
  __forceinline vuint4 operator +(unsigned int  a, const vuint4& b) { return vuint4(a) + b; }

  __forceinline vuint4 operator -(const vuint4& a, const vuint4& b) { return _mm_sub_epi32(a, b); }
  __forceinline vuint4 operator -(const vuint4& a, unsigned int  b) { return a - vuint4(b); }
  __forceinline vuint4 operator -(unsigned int  a, const vuint4& b) { return vuint4(a) - b; }

//#if defined(__SSE4_1__)
//  __forceinline vuint4 operator *(const vuint4& a, const vuint4& b) { return _mm_mullo_epu32(a, b); }
//#else
//  __forceinline vuint4 operator *(const vuint4& a, const vuint4& b) { return vuint4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]); }
//#endif
//  __forceinline vuint4 operator *(const vuint4& a, unsigned int  b) { return a * vuint4(b); }
//  __forceinline vuint4 operator *(unsigned int  a, const vuint4& b) { return vuint4(a) * b; }

  __forceinline vuint4 operator &(const vuint4& a, const vuint4& b) { return _mm_and_si128(a, b); }
  __forceinline vuint4 operator &(const vuint4& a, unsigned int  b) { return a & vuint4(b); }
  __forceinline vuint4 operator &(unsigned int  a, const vuint4& b) { return vuint4(a) & b; }

  __forceinline vuint4 operator |(const vuint4& a, const vuint4& b) { return _mm_or_si128(a, b); }
  __forceinline vuint4 operator |(const vuint4& a, unsigned int  b) { return a | vuint4(b); }
  __forceinline vuint4 operator |(unsigned int  a, const vuint4& b) { return vuint4(a) | b; }

  __forceinline vuint4 operator ^(const vuint4& a, const vuint4& b) { return _mm_xor_si128(a, b); }
  __forceinline vuint4 operator ^(const vuint4& a, unsigned int  b) { return a ^ vuint4(b); }
  __forceinline vuint4 operator ^(unsigned int  a, const vuint4& b) { return vuint4(a) ^ b; }

  __forceinline vuint4 operator <<(const vuint4& a, unsigned int n) { return _mm_slli_epi32(a, n); }
  __forceinline vuint4 operator >>(const vuint4& a, unsigned int n) { return _mm_srli_epi32(a, n); }

  __forceinline vuint4 sll (const vuint4& a, unsigned int b) { return _mm_slli_epi32(a, b); }
  __forceinline vuint4 sra (const vuint4& a, unsigned int b) { return _mm_srai_epi32(a, b); }
  __forceinline vuint4 srl (const vuint4& a, unsigned int b) { return _mm_srli_epi32(a, b); }
  
  ////////////////////////////////////////////////////////////////////////////////
  /// Assignment Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vuint4& operator +=(vuint4& a, const vuint4& b) { return a = a + b; }
  __forceinline vuint4& operator +=(vuint4& a, unsigned int  b) { return a = a + b; }
  
  __forceinline vuint4& operator -=(vuint4& a, const vuint4& b) { return a = a - b; }
  __forceinline vuint4& operator -=(vuint4& a, unsigned int  b) { return a = a - b; }

//#if defined(__SSE4_1__)
//  __forceinline vuint4& operator *=(vuint4& a, const vuint4& b) { return a = a * b; }
//  __forceinline vuint4& operator *=(vuint4& a, unsigned int  b) { return a = a * b; }
//#endif
  
  __forceinline vuint4& operator &=(vuint4& a, const vuint4& b) { return a = a & b; }
  __forceinline vuint4& operator &=(vuint4& a, unsigned int  b) { return a = a & b; }
  
  __forceinline vuint4& operator |=(vuint4& a, const vuint4& b) { return a = a | b; }
  __forceinline vuint4& operator |=(vuint4& a, unsigned int  b) { return a = a | b; }
  
  __forceinline vuint4& operator <<=(vuint4& a, unsigned int  b) { return a = a << b; }
  __forceinline vuint4& operator >>=(vuint4& a, unsigned int  b) { return a = a >> b; }

  ////////////////////////////////////////////////////////////////////////////////
  /// Comparison Operators + Select
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vboolf4 operator ==(const vuint4& a, const vuint4& b) { return _mm_castsi128_ps(_mm_cmpeq_epi32(a, b)); }
  __forceinline vboolf4 operator !=(const vuint4& a, const vuint4& b) { return !(a == b); }
  //__forceinline vboolf4 operator < (const vuint4& a, const vuint4& b) { return _mm_castsi128_ps(_mm_cmplt_epu32(a, b)); }
  //__forceinline vboolf4 operator >=(const vuint4& a, const vuint4& b) { return !(a <  b); }
  //__forceinline vboolf4 operator > (const vuint4& a, const vuint4& b) { return _mm_castsi128_ps(_mm_cmpgt_epu32(a, b)); }
  //__forceinline vboolf4 operator <=(const vuint4& a, const vuint4& b) { return !(a >  b); }

  __forceinline vboolf4 operator ==(const vuint4& a, unsigned int  b) { return a == vuint4(b); }
  __forceinline vboolf4 operator ==(unsigned int  a, const vuint4& b) { return vuint4(a) == b; }

  __forceinline vboolf4 operator !=(const vuint4& a, unsigned int  b) { return a != vuint4(b); }
  __forceinline vboolf4 operator !=(unsigned int  a, const vuint4& b) { return vuint4(a) != b; }

  //__forceinline vboolf4 operator < (const vuint4& a, unsigned int  b) { return a <  vuint4(b); }
  //__forceinline vboolf4 operator < (unsigned int  a, const vuint4& b) { return vuint4(a) <  b; }

  //__forceinline vboolf4 operator >=(const vuint4& a, unsigned int  b) { return a >= vuint4(b); }
  //__forceinline vboolf4 operator >=(unsigned int  a, const vuint4& b) { return vuint4(a) >= b; }

  //__forceinline vboolf4 operator > (const vuint4& a, unsigned int  b) { return a >  vuint4(b); }
  //__forceinline vboolf4 operator > (unsigned int  a, const vuint4& b) { return vuint4(a) >  b; }

  //__forceinline vboolf4 operator <=(const vuint4& a, unsigned int  b) { return a <= vuint4(b); }
  //__forceinline vboolf4 operator <=(unsigned int  a, const vuint4& b) { return vuint4(a) <= b; }

  __forceinline vboolf4 eq(const vuint4& a, const vuint4& b) { return a == b; }
  __forceinline vboolf4 ne(const vuint4& a, const vuint4& b) { return a != b; }
  //__forceinline vboolf4 lt(const vuint4& a, const vuint4& b) { return a <  b; }
  //__forceinline vboolf4 ge(const vuint4& a, const vuint4& b) { return a >= b; }
  //__forceinline vboolf4 gt(const vuint4& a, const vuint4& b) { return a >  b; }
  //__forceinline vboolf4 le(const vuint4& a, const vuint4& b) { return a <= b; }

  __forceinline vboolf4 eq(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a == b); }
  __forceinline vboolf4 ne(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a != b); }
  //__forceinline vboolf4 lt(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a <  b); }
  //__forceinline vboolf4 ge(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a >= b); }
  //__forceinline vboolf4 gt(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a >  b); }
  //__forceinline vboolf4 le(const vboolf4& mask, const vuint4& a, const vuint4& b) { return mask & (a <= b); }

  template<int mask>
  __forceinline vuint4 select(const vuint4& t, const vuint4& f) {
#if defined(__SSE4_1__) 
    return _mm_castps_si128(_mm_blend_ps(_mm_castsi128_ps(f), _mm_castsi128_ps(t), mask));
#else
    return select(vboolf4(mask), t, f);
#endif    
  }

/*#if defined(__SSE4_1__)
  __forceinline vuint4 min(const vuint4& a, const vuint4& b) { return _mm_min_epu32(a, b); }
  __forceinline vuint4 max(const vuint4& a, const vuint4& b) { return _mm_max_epu32(a, b); }

#else
  __forceinline vuint4 min(const vuint4& a, const vuint4& b) { return select(a < b,a,b); }
  __forceinline vuint4 max(const vuint4& a, const vuint4& b) { return select(a < b,b,a); }
#endif

  __forceinline vuint4 min(const vuint4& a, unsigned int  b) { return min(a,vuint4(b)); }
  __forceinline vuint4 min(unsigned int  a, const vuint4& b) { return min(vuint4(a),b); }
  __forceinline vuint4 max(const vuint4& a, unsigned int  b) { return max(a,vuint4(b)); }
  __forceinline vuint4 max(unsigned int  a, const vuint4& b) { return max(vuint4(a),b); }*/

  ////////////////////////////////////////////////////////////////////////////////
  // Movement/Shifting/Shuffling Functions
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline vuint4 unpacklo(const vuint4& a, const vuint4& b) { return _mm_castps_si128(_mm_unpacklo_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b))); }
  __forceinline vuint4 unpackhi(const vuint4& a, const vuint4& b) { return _mm_castps_si128(_mm_unpackhi_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b))); }

  template<int i0, int i1, int i2, int i3>
  __forceinline vuint4 shuffle(const vuint4& v) {
    return _mm_shuffle_epi32(v, _MM_SHUFFLE(i3, i2, i1, i0));
  }

  template<int i0, int i1, int i2, int i3>
  __forceinline vuint4 shuffle(const vuint4& a, const vuint4& b) {
    return _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b), _MM_SHUFFLE(i3, i2, i1, i0)));
  }

#if defined(__SSE3__)
  template<> __forceinline vuint4 shuffle<0, 0, 2, 2>(const vuint4& v) { return _mm_castps_si128(_mm_moveldup_ps(_mm_castsi128_ps(v))); }
  template<> __forceinline vuint4 shuffle<1, 1, 3, 3>(const vuint4& v) { return _mm_castps_si128(_mm_movehdup_ps(_mm_castsi128_ps(v))); }
  template<> __forceinline vuint4 shuffle<0, 1, 0, 1>(const vuint4& v) { return _mm_castpd_si128(_mm_movedup_pd (_mm_castsi128_pd(v))); }
#endif

  template<int i>
  __forceinline vuint4 shuffle(const vuint4& v) {
    return shuffle<i,i,i,i>(v);
  }

#if defined(__SSE4_1__)
  template<int src> __forceinline unsigned int extract(const vuint4& b) { return _mm_extract_epi32(b, src); }
  template<int dst> __forceinline vuint4 insert(const vuint4& a, const unsigned b) { return _mm_insert_epi32(a, b, dst); }
#else
  template<int src> __forceinline unsigned int extract(const vuint4& b) { return b[src&3]; }
  template<int dst> __forceinline vuint4 insert(const vuint4& a, const unsigned b) { vuint4 c = a; c[dst&3] = b; return c; }
#endif

  template<> __forceinline unsigned int extract<0>(const vuint4& b) { return _mm_cvtsi128_si32(b); }

  __forceinline unsigned int toScalar(const vuint4& v) { return _mm_cvtsi128_si32(v); }

  ////////////////////////////////////////////////////////////////////////////////
  /// Output Operators
  ////////////////////////////////////////////////////////////////////////////////

  __forceinline embree_ostream operator <<(embree_ostream cout, const vuint4& a) {
    return cout << "<" << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ">";
  }
}

#undef vboolf
#undef vboold
#undef vint
#undef vuint
#undef vllong
#undef vfloat
#undef vdouble
