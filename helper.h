#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "config.h"

#if ULONG_MAX == 4294967295U

typedef uint32_t word_t;
typedef unsigned int dword_t __attribute__((mode(DI)));
#define WORD_BITS 32

#else

typedef uint64_t word_t;
typedef unsigned int dword_t __attribute__((mode(TI)));
#define WORD_BITS 64

#endif

#if WANT_ASSERT
#define ASSERT assert
#else
#define ASSERT(xxx) 
#endif

typedef word_t * nn_t;
typedef const word_t * nn_src_t;

typedef long len_t;
typedef long bits_t;

typedef void * rand_t;

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

/*
   Computes the number of leading zeroes in the binary representation
   of its argument.
*/
#define clz __builtin_clzl

/*
   Precomputes an inverse of d as per the definition of \nu at the
   start of section 3 of Moller-Granlund (see below). Does not 
   require d to be normalised. 
*/
static inline
void precompute_inverse1(preinv1_t * inv, word_t d)
{
   dword_t t;
   word_t norm = clz(d);
   
   d <<= norm;
   t = (~(dword_t) 0) - (((dword_t) d) << WORD_BITS);
   
   inv->dinv = t / d;
   inv->norm = norm;
}

/*
   Precomputes an inverse of the leading WORD_BITS of d with leading words 
   d1, d2 (or d1, 0 if d has only one word) as per the definition of \nu at 
   the start of section 3 of Moller-Granlund (see below). Does not require 
   d1, d2 to be normalised. A normalised version of d1, d2 is returned.
*/
static inline
void precompute_inverse1_2(preinv1_2_t * inv, word_t d1, word_t d2)
{
   dword_t t;
   word_t norm = clz(d1);
   
   d1 <<= norm;
   if (norm) d1 += (d2 >> (WORD_BITS - norm));

   t = (~(dword_t) 0) - (((dword_t) d1) << WORD_BITS);
   
   inv->dinv = t / d1;
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
   dword_t u = (dword_t) 1; 
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b1 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b2 = (word_t) u;
   u = (u << WORD_BITS) % (dword_t) d;
   inv->b3 = (word_t) u;
}

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
   Generate a random word in the range [0, m). 
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

