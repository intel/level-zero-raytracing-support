// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "platform.h"

#if defined(__WIN32__)
#include <intrin.h>
#endif

#include <immintrin.h>

#if defined(__WIN32__)
#  if !defined(NOMINMAX)
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

/* normally defined in pmmintrin.h, but we always need this */
#if !defined(_MM_SET_DENORMALS_ZERO_MODE)
#define _MM_DENORMALS_ZERO_ON   (0x0040)
#define _MM_DENORMALS_ZERO_OFF  (0x0000)
#define _MM_DENORMALS_ZERO_MASK (0x0040)
#define _MM_SET_DENORMALS_ZERO_MODE(x) (_mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (x)))
#endif

namespace embree
{
  
////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////
  
#if defined(__WIN32__) && !defined(__INTEL_LLVM_COMPILER)
  
  __forceinline int bsf(int v) {
    unsigned long r = 0; _BitScanForward(&r,v); return r;
  }
  
  __forceinline unsigned bsf(unsigned v) {
    unsigned long r = 0; _BitScanForward(&r,v); return r;
  }
  
#if defined(__X86_64__)
  __forceinline size_t bsf(size_t v) {
    unsigned long r = 0; _BitScanForward64(&r,v); return r;
  }
#endif
  
  __forceinline int bscf(int& v) 
  {
    int i = bsf(v);
    v &= v-1;
    return i;
  }
  
  __forceinline unsigned bscf(unsigned& v) 
  {
    unsigned i = bsf(v);
    v &= v-1;
    return i;
  }
  
#if defined(__X86_64__)
  __forceinline size_t bscf(size_t& v) 
  {
    size_t i = bsf(v);
    v &= v-1;
    return i;
  }
#endif
  
  __forceinline int bsr(int v) {
    unsigned long r = 0; _BitScanReverse(&r,v); return r;
  }
  
  __forceinline unsigned bsr(unsigned v) {
    unsigned long r = 0; _BitScanReverse(&r,v); return r;
  }
  
#if defined(__X86_64__)
  __forceinline size_t bsr(size_t v) {
    unsigned long r = 0; _BitScanReverse64(&r, v); return r;
  }
#endif
  
  __forceinline int lzcnt(const int x)
  {
    if (unlikely(x == 0)) return 32;
    return 31 - bsr(x);    
  }
  

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////
  
#else
  
  __forceinline uint64_t read_tsc()  {
#if defined(__X86_ASM__)
    uint32_t high,low;
    asm volatile ("rdtsc" : "=d"(high), "=a"(low));
    return (((uint64_t)high) << 32) + (uint64_t)low;
#else
    /* Not supported yet, meaning measuring traversal cost per pixel does not work. */
    return 0;
#endif
  }
  
  __forceinline int bsf(int v) {
#if defined(__X86_ASM__)
    int r = 0; asm ("bsf %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return __builtin_ctz(v);
#endif
  }

#if defined(__64BIT__)
  __forceinline unsigned bsf(unsigned v) 
  {
#if defined(__X86_ASM__)
    unsigned r = 0; asm ("bsf %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return __builtin_ctz(v);
#endif
  }
#endif
  
  __forceinline size_t bsf(size_t v) {
#if defined(__X86_ASM__)
    size_t r = 0; asm ("bsf %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return __builtin_ctzl(v);
#endif
  }

  __forceinline int bscf(int& v) 
  {
    int i = bsf(v);
    v &= v-1;
    return i;
  }
  
#if defined(__64BIT__)
  __forceinline unsigned int bscf(unsigned int& v) 
  {
    unsigned int i = bsf(v);
    v &= v-1;
    return i;
  }
#endif
  
  __forceinline size_t bscf(size_t& v) 
  {
    size_t i = bsf(v);
    v &= v-1;
    return i;
  }
  
  __forceinline int bsr(int v) {
#if defined(__X86_ASM__)
    int r = 0; asm ("bsr %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return __builtin_clz(v) ^ 31;
#endif
  }
  
#if defined(__64BIT__)
  __forceinline unsigned bsr(unsigned v) {
#if defined(__X86_ASM__)
    unsigned r = 0; asm ("bsr %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return __builtin_clz(v) ^ 31;
#endif
  }
#endif
  
  __forceinline size_t bsr(size_t v) {
#if defined(__X86_ASM__)
    size_t r = 0; asm ("bsr %1,%0" : "=r"(r) : "r"(v)); return r;
#else
    return (sizeof(v) * 8 - 1) - __builtin_clzl(v);
#endif
  }
  
  __forceinline int lzcnt(const int x)
  {
    if (unlikely(x == 0)) return 32;
    return 31 - bsr(x);    
  }

#endif

#if !defined(__WIN32__)

#if defined(__i386__) && defined(__PIC__)

  __forceinline void __cpuid(int out[4], int op)
  {
    asm volatile ("xchg{l}\t{%%}ebx, %1\n\t"
                  "cpuid\n\t"
                  "xchg{l}\t{%%}ebx, %1\n\t"
                  : "=a"(out[0]), "=r"(out[1]), "=c"(out[2]), "=d"(out[3])
                  : "0"(op));
  }

  __forceinline void __cpuid_count(int out[4], int op1, int op2)
  {
    asm volatile ("xchg{l}\t{%%}ebx, %1\n\t"
                  "cpuid\n\t"
                  "xchg{l}\t{%%}ebx, %1\n\t"
                  : "=a" (out[0]), "=r" (out[1]), "=c" (out[2]), "=d" (out[3])
                  : "0" (op1), "2" (op2));
  }

#elif defined(__X86_ASM__)

  __forceinline void __cpuid(int out[4], int op) {
    asm volatile ("cpuid" : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3]) : "a"(op));
  }

  __forceinline void __cpuid_count(int out[4], int op1, int op2) {
    asm volatile ("cpuid" : "=a"(out[0]), "=b"(out[1]), "=c"(out[2]), "=d"(out[3]) : "a"(op1), "c"(op2));
  }

#endif

#endif
  
////////////////////////////////////////////////////////////////////////////////
/// All Platforms
////////////////////////////////////////////////////////////////////////////////
  
#if defined(__clang__) || defined(__GNUC__)
#if !defined(_mm_undefined_ps)
  __forceinline __m128 _mm_undefined_ps() { return _mm_setzero_ps(); }
#endif
#if !defined(_mm_undefined_si128)
  __forceinline __m128i _mm_undefined_si128() { return _mm_setzero_si128(); }
#endif
#endif

#if defined(__SSE4_2__)
  
  __forceinline int popcnt(int in) {
    return _mm_popcnt_u32(in);
  }
  
  __forceinline unsigned popcnt(unsigned in) {
    return _mm_popcnt_u32(in);
  }
  
#if defined(__64BIT__)
  __forceinline size_t popcnt(size_t in) {
    return _mm_popcnt_u64(in);
  }
#endif
  
#endif
}
