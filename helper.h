/* 
  Copyright (C) 2010, 2013 William Hart
  Copyright (C) 2010 Brian Gladman

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
#include <stdlib.h>
#include <alloca.h>
#include <limits.h>
#include <assert.h>
#include "config.h"
#include "types_arch.h"

#if ULONG_MAX == 4294967295U /* 32 bit unsigned long */

typedef uint32_t word_t;
typedef unsigned int dword_t __attribute__((mode(DI)));
typedef int32_t len_t;
typedef int32_t bits_t;
#define WORD_BITS 32
#define WORD(x) (x##UL)
#define WORD_FMT "%l"
#define LEN_FMT "%ld"
#define BITS_FMT "%ld"

#else /* 64 bit unsigned long */

typedef uint64_t word_t;
typedef unsigned int dword_t __attribute__((mode(TI)));
typedef int64_t len_t;
typedef int64_t bits_t;
#define WORD_BITS 64
#define WORD(x) (x##UL)
#define WORD_FMT "%l"
#define LEN_FMT "%ld"
#define BITS_FMT "%ld"

#endif

#if WANT_ASSERT
#define ASSERT assert
#else
#define ASSERT(xxx) 
#endif

typedef word_t * nn_t;
typedef const word_t * nn_src_t;

typedef word_t preinv1_t;

typedef word_t hensel_preinv1_t;

typedef struct mod_preinv1_t
{
   word_t b1; /* B   mod d */
   word_t b2; /* B^2 mod d */
   word_t b3; /* B^3 mod d */
} mod_preinv1_t;

#define TMP_INIT \
   typedef struct __tmp_struct { \
      void * block; \
      struct __tmp_struct * next; \
   } __tmp_t; \
   __tmp_t * __tmp_root; \
   __tmp_t * __t

#define TMP_START \
   __tmp_root = NULL

#define TMP_ALLOC_BYTES(size) \
   ((size) > 8192 ? \
      (__t = alloca(sizeof(__tmp_t)), \
       __t->next = __tmp_root, \
       __tmp_root = __t, \
       __t->block = malloc(size)) : \
      alloca(size))

#define TMP_ALLOC(size) \
   TMP_ALLOC_BYTES(sizeof(word_t)*size)

#define TMP_END \
   while (__tmp_root) { \
      free(__tmp_root->block); \
      __tmp_root = __tmp_root->next; \
   }

#include "helper_arch.h"
#include "rand.h"

/**********************************************************************
 
    Helper functions/macros

**********************************************************************/

#ifndef HAVE_ARCH_INTRINSICS

#define high_zero_bits __builtin_clzl

#endif

#ifndef HAVE_ARCH_divapprox21_preinv1

/*
   Given a double word u, a normalised divisor d and a precomputed
   inverse dinv of d, computes an approximate quotient of u by d.
   The quotient will be either correct, or up to 3 less than the
   actual value.
*/
#define divapprox21_preinv1(q, u_hi, u_lo, d, dinv) \
   do { \
      dword_t __q1 = (dword_t) u_hi * (dword_t) (dinv) \
                  + (((dword_t) u_hi) << WORD_BITS) + (dword_t) u_lo; \
      const dword_t __q0 = (dword_t) u_lo * (dword_t) (dinv); \
      __q1 += (dword_t) ((__q0) >> WORD_BITS); \
      (q) = (__q1 >> WORD_BITS); \
   } while (0)

#endif

#ifndef HAVE_ARCH_divapprox21_preinv1

/*
   Given a double word u, a normalised divisor d and a precomputed
   inverse dinv of d, computes the quotient and remainder of u by d.
*/
#define divrem21_preinv1(q, r, u_hi, u_lo, d, dinv) \
   do { \
      const dword_t __u = (((dword_t) u_hi) << WORD_BITS) + (dword_t) u_lo; \
      dword_t __r, __q1 = (dword_t) u_hi * (dword_t) (dinv) + __u; \
      const dword_t __q0 = (dword_t) u_lo * (dword_t) (dinv); \
      __q1 += (dword_t) ((__q0) >> WORD_BITS); \
      (q) = (__q1 >> WORD_BITS); \
      __r = __u - (dword_t) (d) * (dword_t) (q); \
      while ((word_t) (__r >> WORD_BITS) || ((word_t) __r >= (d))) \
      { \
         __r -= (dword_t) (d); \
         (q)++; \
      } \
      (r) = (word_t) __r; \
   } while (0)

#endif

#ifndef HAVE_ARCH_precompute_inverse1

/*
   Precomputes an inverse of d. The value of the inverse is
   2^WORD_BITS / (d + 1) - 2^(WORD_BITS). We assume d is
   normalised, i.e. the most significant bit of d is set.
*/
static inline
preinv1_t precompute_inverse1(word_t d)
{
   ASSERT(d != 0); 
   
   d++;

   if (d == 0)
      return 0;

   return (word_t) ((((dword_t) -d) << WORD_BITS) / (dword_t) d);
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

/*
   Print a string containing format specifiers. This is identical to
   printf with the exception of the additional format specifiers
   %w : print a word_t
   %m : print a len_t
   %b : print a bits_t
*/
void bsdnt_printf(const char * str, ...);

#endif
