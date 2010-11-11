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

#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "config.h"
#include "types_arch.h"

#ifndef HAVE_ARCH_TYPES

#if ULONG_MAX == 4294967295U /* 32 bit unsigned long */

typedef uint32_t word_t;
typedef unsigned int dword_t __attribute__((mode(DI)));
typedef int32_t len_t;
typedef int32_t bits_t;
#define WORD_BITS 32
#define WORD(x) (x##UL)

#else /* 64 bit unsigned long */

typedef uint64_t word_t;
typedef unsigned int dword_t __attribute__((mode(TI)));
typedef int64_t len_t;
typedef int64_t bits_t;
#define WORD_BITS 64
#define WORD(x) (x##UL)

#endif

#if WANT_ASSERT
#define ASSERT assert
#else
#define ASSERT(xxx) 
#endif

#endif

typedef word_t * nn_t;
typedef const word_t * nn_src_t;

typedef struct preinv1_t
{
   bits_t norm; /* the number of leading zero bits in d */
   word_t dinv; /* the precomputed inverse of d (see below) */
} preinv1_t;

typedef struct preinv1_2_t
{
   bits_t norm; /* the number of leading zero bits in d */
   word_t dinv; /* the precomputed inverse of d1 (see below) */
   word_t d1; /* the normalised leading WORD_BITS of d */
} preinv1_2_t;

typedef word_t hensel_preinv1_t;

typedef struct mod_preinv1_t
{
   word_t b1; /* B   mod d */
   word_t b2; /* B^2 mod d */
   word_t b3; /* B^3 mod d */
} mod_preinv1_t;

#include "helper_arch.h"
#include "rand/bsdnt_rand.h"

/**********************************************************************
 
    Helper functions/macros

**********************************************************************/

#ifndef HAVE_ARCH_INTRINSICS

#define high_zero_bits __builtin_clzl

#endif

#ifndef HAVE_ARCH_divrem21_preinv1

/*
   Given a double word u, a normalised divisor d and a precomputed
   inverse dinv of d, computes the quotient and remainder of u by d.
*/
#define divrem21_preinv1(q, r, u, d, dinv) \
   do { \
      dword_t __q = ((u)>>WORD_BITS) * (dword_t) (dinv) + u; \
      word_t __q1 = (word_t)(__q >> WORD_BITS) + 1; \
      word_t __q0 = (word_t) __q; \
      word_t __r1 = (word_t)(u) - __q1*(d); \
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

#endif

#ifndef HAVE_ARCH_precompute_inverse1

/*
   Precomputes an inverse of d as per the definition of \nu at the
   start of section 3 of Moller-Granlund (see below). Does not 
   require d to be normalised, but d must not be 0. 
*/
static inline
void precompute_inverse1(preinv1_t * inv, word_t d)
{
   dword_t t;
   word_t norm = high_zero_bits(d);
   d <<= norm;

   ASSERT(d != 0);

   t = (~(dword_t) 0) - (((dword_t) d) << WORD_BITS);
   inv->dinv = t / d;

   inv->norm = norm;
}

#endif

#ifndef HAVE_ARCH_precompute_inverse1_2

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

   t = (~(dword_t) 0) - (((dword_t) d1) << WORD_BITS);
   inv->dinv = t / d1;

   inv->norm = norm;
   inv->d1 = d1;
}

#endif

#ifndef HAVE_ARCH_precompute_mod_inverse1

/*
   Precomputes B, B^2, B^3 mod d. Requires that d is not zero.
*/
static inline
void precompute_mod_inverse1(mod_preinv1_t * inv, word_t d)
{
   dword_t u = (dword_t) 1; 

   ASSERT(d != 0);
   
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b1 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b2 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b3 = (word_t) u;
}

#endif

#ifndef HAVE_ARCH_precompute_hensel_inverse1

/*
   Precomputes a Hensel inverse of d, i.e. a value dinv such that
   d * dinv = 1 mod B. The algorithm is via Hensel lifting.
   Requires that d is odd.
*/
static inline
void precompute_hensel_inverse1(hensel_preinv1_t * inv, word_t d)
{
   word_t v = 1; /* initial solution modulo 2 */
   word_t u;

   ASSERT(d & (word_t) 1);

   while ((u = d * v) != 1)
      v += (1 - u) * v;
   
   (*inv) = v;
}

#endif

/**********************************************************************
 
    Printing functions

**********************************************************************/

/*
   Print a word in hexacdecimal.
*/
void printx_word(word_t a);

#endif
