/* 
  Copyright (C) 2010, William Hart
  Copyright (C) 2010, Brian Gladman

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
	 documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef HELPER_ARCH_H
#define HELPER_ARCH_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "config.h"

#define HAVE_ARCH_INTRINSICS

#include <crtdbg.h>
#include <intrin.h>

word_t div_128_by_64(dword_t *n,  word_t d, word_t *r);

#pragma intrinsic(_umul128)
#define mul_64_by_64 _umul128

#pragma intrinsic(_BitScanReverse64)
__inline uint32_t high_zero_bits(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanReverse64(&pos, x);
	return WORD_BITS - 1 - pos;
}

#pragma intrinsic(_BitScanForward64)
__inline uint32_t low_zero_bits(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanForward64(&pos, x);
	return pos;
}

#define HAVE_ARCH_divrem21_preinv1
/*
   Given a double word u, a normalised divisor d and a precomputed
   inverse dinv of d, computes the quotient and remainder of u by d.
*/
#define divrem21_preinv1(q, r, u, d, dinv) \
   do { \
      dword_t __q; \
      word_t __q0, __q1, __r1; \
	  __q.lo = mul_64_by_64(u.hi, dinv, &__q.hi) + u.lo; \
	  __q.hi += u.hi + (__q.lo < u.lo ? 1 : 0); \
	  __q1 = __q.hi + 1; \
      __r1 = u.lo - (word_t)(__q1 * (d)); \
  	  __q0 = __q.lo; \
      if (__r1 >= __q0) \
      { \
         __q1--; \
         __r1 += (d); \
      } \
      if (__r1 >= (d)) \
      { \
         (q) = __q1 + 1; \
         (r) = __r1 - (d); \
      } else \
      { \
         (q) = __q1; \
         (r) = __r1; \
      } \
   } while (0)

#define HAVE_ARCH_precompute_inverse1
/*
   Precomputes an inverse of d as per the definition of \nu at the
   start of section 3 of Moller-Granlund (see below). Does not 
   require d to be normalised, but d must not be 0. 
*/
inline
void precompute_inverse1(preinv1_t * inv, word_t d)
{
   dword_t t;
   word_t norm = high_zero_bits(d);
   d <<= norm;

   ASSERT(d != 0);

   t.hi = ((word_t)-1) - d;
   t.lo = ((word_t)-1);
   inv->dinv = div_128_by_64(&t, d, &t.hi);
   inv->norm = norm;
}

#define HAVE_ARCH_precompute_inverse1_2
/*
   Precomputes an inverse of the leading WORD_BITS of d with leading words 
   d1, d2 (or d1, 0 if d has only one word) as per the definition of \nu at 
   the start of section 3 of Moller-Granlund (see below). Does not require 
   d1, d2 to be normalised. A normalised version of d1, d2 is returned.
   Requires that d1 be nonzero.
*/
static inline
void precompute_inverse1_2(preinv1_2_t * inv, word_t d1, word_t d2)
{
   dword_t t;
   word_t norm = high_zero_bits(d1);

   ASSERT(d1 != 0);

   d1 <<= norm;
   if (norm) d1 += (d2 >> (WORD_BITS - norm));

   t.hi = ((word_t)-1) - d1;
   t.lo = ((word_t)-1);
   inv->dinv = div_128_by_64(&t, d1, &t.hi);

   inv->norm = norm;
   inv->d1 = d1;
}

#define HAVE_ARCH_precompute_mod_inverse1

static inline
void precompute_mod_inverse1(mod_preinv1_t * inv, word_t d)
{
	dword_t u;

   ASSERT(d != 0);
   
	u.hi = 1;
	u.lo = 0;
	div_128_by_64(&u, d, &u.hi);
	inv->b1 = u.hi;
	u.lo = 0;
	div_128_by_64(&u, d, &u.hi);
	inv->b2 = u.hi;
	u.lo = 0;
	div_128_by_64(&u, d, &u.hi);
	inv->b3 = u.hi;
}

#endif
