#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "config.h"
#include "rand/bsdnt_rand.h"

#ifdef _MSC_VER

# ifdef _WIN64
    typedef struct uint128_t
    {	uint64_t lo, hi;
    } uint128_t;
    typedef uint64_t word_t;
    typedef uint128_t dword_t;
    typedef int64_t len_t;
    typedef int64_t bits_t;
#   define WORD_BITS 64
# else
    typedef uint32_t word_t;
    typedef uint64_t dword_t;
    typedef int32_t len_t;
    typedef int32_t bits_t;
#   define WORD_BITS 32
# endif

#else

# if ULONG_MAX == 4294967295U
    typedef uint32_t word_t;
    typedef unsigned int dword_t __attribute__((mode(DI)));
    typedef int32_t len_t;
    typedef int32_t bits_t;
#   define WORD_BITS 32
# else
    typedef uint64_t word_t;
    typedef unsigned int dword_t __attribute__((mode(TI)));
    typedef int64_t len_t;
    typedef int64_t bits_t;
#   define WORD_BITS 64
# endif

#endif

#if WANT_ASSERT
#define ASSERT assert
#else
#define ASSERT(xxx) 
#endif

typedef word_t * nn_t;
typedef const word_t * nn_src_t;

typedef struct preinv1_t
{
   word_t norm; /* the number of leading zero bits in d */
   word_t dinv; /* the precomputed inverse of d (see below) */
} preinv1_t;

typedef struct preinv1_2_t
{
   word_t norm; /* the number of leading zero bits in d */
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

/**********************************************************************
 
    Helper functions/macros

**********************************************************************/

#ifdef _MSC_VER

#include <crtdbg.h>
#include <intrin.h>

#define inline __inline
word_t div_128_by_64(dword_t *n,  word_t d, word_t *r);

#if WORD_BITS == 32

#pragma intrinsic(_BitScanReverse)
__inline uint32_t count_leading_zeros(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanReverse(&pos, x);
	return WORD_BITS - 1 - pos;
}

#pragma intrinsic(_BitScanForward)
__inline uint32_t count_trailing_zeros(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanForward(&pos, x);
	return pos;
}

#endif

#if WORD_BITS == 64

#pragma intrinsic(_umul128)
#define mul_64_by_64 _umul128

#pragma intrinsic(_BitScanReverse64)
__inline uint32_t count_leading_zeros(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanReverse64(&pos, x);
	return WORD_BITS - 1 - pos;
}

#pragma intrinsic(_BitScanForward64)
__inline uint32_t count_trailing_zeros(word_t x)
{
	uint32_t pos;
	_ASSERTE(x != 0);
	_BitScanForward64(&pos, x);
	return pos;
}

#endif

#endif

/*
   Computes the number of leading zeroes in the binary representation
   of its argument.
*/
#ifndef _MSC_VER
#  define clz __builtin_clzl
#else
#  define clz count_leading_zeros
#endif

/*
   Precomputes an inverse of d as per the definition of \nu at the
   start of section 3 of Moller-Granlund (see below). Does not 
   require d to be normalised, but d must not be 0. 
*/
static inline
void precompute_inverse1(preinv1_t * inv, word_t d)
{
   dword_t t;
   word_t norm = clz(d);
   d <<= norm;

   ASSERT(d != 0);

#if defined( _MSC_VER ) && WORD_BITS == 64
   t.hi = ((word_t)-1) - d;
   t.lo = ((word_t)-1);
   inv->dinv = div_128_by_64(&t, d, &t.hi);
#else
   t = (~(dword_t) 0) - (((dword_t) d) << WORD_BITS);
   inv->dinv = t / d;
#endif

   inv->norm = norm;
}

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
   word_t norm = clz(d1);

   ASSERT(d1 != 0);

   d1 <<= norm;
   if (norm) d1 += (d2 >> (WORD_BITS - norm));

#if defined( _MSC_VER ) && WORD_BITS == 64
   t.hi = ((word_t)-1) - d1;
   t.lo = ((word_t)-1);
   inv->dinv = div_128_by_64(&t, d1, &t.hi);
#else
   t = (~(dword_t) 0) - (((dword_t) d1) << WORD_BITS);
   inv->dinv = t / d1;
#endif

   inv->norm = norm;
   inv->d1 = d1;
}

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

/*
   Precomputes B, B^2, B^3 mod d. Requires that d is not zero.
*/
static inline
void precompute_mod_inverse1(mod_preinv1_t * inv, word_t d)
{
#if defined( _MSC_VER ) && WORD_BITS == 64
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
#else
   dword_t u = (dword_t) 1; 

   ASSERT(d != 0);
   
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b1 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b2 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b3 = (word_t) u;
#endif
}

/*
   Given a double word u, a normalised divisor d and a precomputed
   inverse dinv of d, computes the quotient and remainder of u by d.
*/
#if !defined( _MSC_VER ) || WORD_BITS != 64

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

#else

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

#endif

/**********************************************************************
 
    Random generation

**********************************************************************/

/*
   Initialise a random state for use. 
*/
void randinit(rand_t state);

/*
   Clear a random state after use. 
*/
void randclear(rand_t state);

/*
   Generate a random word of data. 
*/
word_t randword(rand_t state);

/*
   Generate a random word in the range [0, m). Requires m to not
   be zero.
*/
word_t randint(word_t m, rand_t state);

/**********************************************************************
 
    Printing functions

**********************************************************************/

/*
   Print a word in hexacdecimal.
*/
void printx_word(word_t a);

#endif

