#ifndef BSDNT_NN_H
#define BSDNT_NN_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#if ULONG_MAX == 4294967295U

typedef uint32_t word_t;
typedef unsigned int dword_t __attribute__((mode(DI)));
#define WORD_BITS 32

#else

typedef uint64_t word_t;
typedef unsigned int dword_t __attribute__((mode(TI)));
#define WORD_BITS 64

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
   require d to be normalised. A normalised version of d is returned.
*/
static inline
word_t precompute_inverse1(preinv1_t * inv, word_t d)
{
   dword_t t;
   word_t norm = clz(d);
   
   d <<= norm;
   t = (~(dword_t) 0) - (((dword_t) d) << WORD_BITS);
   
   inv->dinv = t / d;
   inv->norm = norm;

   return d;
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

/*
   Set the first m limbs of a to random words. 
*/
void nn_random(nn_t a, rand_t state, len_t m);

/**********************************************************************
 
    Memory management

**********************************************************************/

/*
   Allocates an array of m words and returns it. Despite the
   name, the limbs are not initialised to zero.
*/
static inline
nn_t nn_init(len_t m)
{
   if (m) return malloc(m*sizeof(word_t));
   else return NULL;
}

/*
   Free the memory used by the nn_t a.
*/
static inline
void nn_clear(nn_t a)
{
   if (a) free(a);
}

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

/*
   Copy the m limbs at b to a.
*/
static inline
void nn_copy(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = b[i];
}

/*
   Set the m limbs at b to 0.
*/
static inline
void nn_zero(nn_t a, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = 0;
}

/*
   The pair {a, m} is said to be normalised if either m is
   zero, or a[m-1] is non-zero. In other words, {a, m} has
   no leading zero limbs. This function normalises {a, m}
   by returning the largest value m0 <= m for which {a, m0}
   is normalised.
*/
static inline
len_t nn_normalise(nn_t a, len_t m)
{
   while ((m != 0) && (a[m - 1] == 0))
      m--;

   return m;
}

/**********************************************************************
 
    Linear arithmetic functions

**********************************************************************/

/*
   Set a to the twos complement negation of b, where b is m words 
   in length. Return any borrow. The word ci is a carry-in. It is
   effectively subtracted from the result.
*/
word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci);

/*
   Set a to the twos complement negation of b, where b is m words
   in length. Return any borrow.
*/
#define nn_neg(a, b, m) \
    nn_neg_c(a, b, m, 0)

/*
   Set a to the twos complement negation of b, where b is m words
   in length. The word ci is a carry-in. It is effectively subtracted
   from the result. The borrow is written out to a[m] and returned. 
*/
static inline
word_t nn_s_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
{
   return (a[m] = -nn_neg_c(a, b, m, ci));
}

/*
   Set a to the twos complement negation of b, where b is m words
   in length. The borrow is written out to a[m].
*/
#define nn_s_neg(a, b, m) \
   nn_s_neg_c(a, b, m, 0)

/*
   Set a = b + c where b is m words in length, and c is a word. 
   Return any carry out. 
*/
word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b + c where b is m words in length, and c is a word. 
   Write any carry out to a[m]. If a and b are aliased, the 
   carry out is added to a[m], otherwise it is written to a[m].
*/
#define nn_s_add1(a, b, m, c) \
   do { \
      if ((a) == (b)) \
         (a)[m] += nn_add1(a, b, m, c); \
      else \
         (a)[m] = nn_add1(a, b, m, c); \
   } while (0)

/*
   Set a = b - c where b is m words in length, and c is a word. 
   Return any borrow out. 
*/
word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b - c where b is m words in length, and c is a word. 
   Write any borrow out to a[m]. If a and b are aliased, the 
   borrow out is subtracted from a[m], otherwise it is written 
   to a[m].
*/
#define nn_s_sub1(a, b, m, c) \
   do { \
      if ((a) == (b)) \
         (a)[m] -= nn_sub1(a, b, m, c); \
      else \
         (a)[m] = -nn_sub1(a, b, m, c); \
   } while (0)

/*
   Set a = b + c + ci where b and c are both m words in length,
   ci is a "carry in". Return any carry out. 
*/
word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci);

/*
   Set a = b + c where b and c are both m words in length. Return 
   any carry out. 
*/
#define nn_add_m(a, b, c, m) \
   nn_add_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b + c + ci where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_s_add_mc(a, b, c, m, ci) \
   do { \
      if ((a) == (b)) \
         (a)[m] += nn_add_mc(a, b, c, m, ci); \
      else \
         (a)[m] = nn_add_mc(a, b, c, m, ci); \
   } while (0)

/*
   Set a = b + c where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_s_add_m(a, b, c, m) \
   nn_s_add_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length,
   bi is a "borrow". Return any borrow. 
*/
word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi);

/*
   Set a = b - c where b and c are both m words in length. Return 
   any borrow. 
*/
#define nn_sub_m(a, b, c, m) \
   nn_sub_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the boorow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_s_sub_mc(a, b, c, m, bi) \
   do { \
      if ((a) == (b)) \
         (a)[m] -= nn_sub_mc(a, b, c, m, bi); \
      else \
         (a)[m] = -nn_sub_mc(a, b, c, m, bi); \
   } while (0)

/*
   Set a = b - c where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the borrow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_s_sub_m(a, b, c, m) \
   nn_s_sub_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "carry in". We return the carry out. 
*/
static inline
word_t nn_add_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ci = nn_add_mc(a, b, c, cm, ci);
   return nn_add1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   and bm >= cm. We return the carry out. 
*/
#define nn_add(a, b, bm, c, cm) \
   nn_add_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "carry in". We write the carry out to a[bm].
   If a and b are aliased the carry out is added to a[bm], otherwise
   it is written there.
*/
#define nn_s_add_c(a, b, bm, c, cm, ci) \
   do { \
      if ((a) == (b)) \
         (a)[bm] += nn_add_c(a, b, bm, c, cm, ci); \
      else \
         (a)[bm] = nn_add_c(a, b, bm, c, cm, ci); \
   } while (0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   and bm >= cm. We write the carry out to a[bm]. If a and b are 
   aliased the carry out is added to a[bm], otherwise it is written 
   there.
*/
#define nn_s_add(a, b, bm, c, cm) \
   nn_s_add_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "borrow in". We return any borrow out. 
*/
static inline
word_t nn_sub_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ci = nn_sub_mc(a, b, c, cm, ci);
   return nn_sub1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   and bm >= cm. We return any borrow out. 
*/
#define nn_sub(a, b, bm, c, cm) \
   nn_sub_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "borrow in". We write the borrow out to a[bm].
   If a and b are aliased the borrow out is subtracted from a[bm], 
   otherwise it is written there.
*/
#define nn_s_sub_c(a, b, bm, c, cm, ci) \
   do { \
      if ((a) == (b)) \
         (a)[bm] -= nn_sub_c(a, b, bm, c, cm, ci); \
      else \
         (a)[bm] = -nn_sub_c(a, b, bm, c, cm, ci); \
   } while (0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   and bm >= cm. We write the borrow out to a[bm]. If a and b are 
   aliased the borrow out is subtracted from a[bm], otherwise it is 
   written there.
*/
#define nn_s_sub(a, b, bm, c, cm) \
   nn_s_sub_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length,
   ci is a "carry in". Return any carry out. Assumes 0 <= bits
   < WORD_BITS.
*/
word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b << bits) where b is m words in length. Return 
   any carry out. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shl(a, b, m, bits) \
   nn_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length, ci is
   a "carry in", and write the carry out to a[m] and return it. 
   Assumes 0 <= bits < WORD_BITS.
*/
static inline
word_t nn_s_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   return (a[m] = nn_shl_c(a, b, m, bits, ci));
}

/*
   Set a = (b << bits) where b is m words in length, writing the 
   carry out to a[m]. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_s_shl(a, b, m, bits) \
   nn_s_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, ci is a "carry in". Return any carry out from the low
   end. Assumes 0 <= bits < WORD_BITS.
*/
word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b >> bits) where b is m words in length. Return 
   any carry out from the low end. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shr(a, b, m, bits) \
   nn_shr_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, and ci is a[m]*2^(WORD_BITS - bits). Assumes 0 <= bits < 
   WORD_BITS.
*/
static inline
word_t nn_r_shr(nn_t a, nn_src_t b, len_t m, bits_t bits)
{
   if (bits)
      return nn_shr_c(a, b, m, bits, (b)[m] << (WORD_BITS - (bits))); \
   else
      return nn_shr_c(a, b, m, bits, (word_t) 0); \
}

/*
   Set a = b * c + ci where b is m words in length, c is a word and
   ci is a "carry in". Return any carry out. 
*/
word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = b * c where b is m words in length and c is a word. 
   Return any carry out.
*/
#define nn_mul1(a, b, m, c) \
   nn_mul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = b * c + ci where b is m words in length, c is a word and
   ci is a "carry in". Write any carry out to a[m] and return it.
*/
static inline
word_t nn_s_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   return (a[m] = nn_mul1_c(a, b, m, c, ci)); 
}

/*
   Set a = b * c where b is m words in length and c is a word.
   Write any carry out to a[m]. 
*/
#define nn_s_mul1(a, b, m, c) \
   nn_s_mul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is returned. 
*/
word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a + b * c where a and b are m words in length, and c 
   is a word. The carry out is returned. 
*/
#define nn_addmul1(a, b, m, c) \
   nn_addmul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is added to
   a[m]. 
*/
#define nn_s_addmul1_c(a, b, m, c, ci) \
   do { \
      (a)[m] += nn_addmul1_c(a, b, m, c, ci); \
   } while (0)

/*
   Set a = a + b * c where a and b are m words in length and c 
   is a word. The carry out is added to a[m].  
*/
#define nn_s_addmul1(a, b, m, c) \
   nn_s_addmul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a - b * c - ci where a and b are m words in length, c 
   is a word and ci is a "borrow in". The borrow out is returned. 
*/
word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a - b * c where a and b are m words in length, and c 
   is a word. The borrow out is returned. 
*/
#define nn_submul1(a, b, m, c) \
   nn_submul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a - b * c - ci where a and b are m words in length, c 
   is a word and ci is a "borrow in". The borrow out is subtracted
   from a[m]. 
*/
#define nn_s_submul1_c(a, b, m, c, ci) \
   do { \
      (a)[m] -= nn_submul1_c(a, b, m, c, ci); \
   } while (0)

/*
   Set a = a - b * c where a and b are m words in length and c 
   is a word. The borrow out is subtracted from a[m].  
*/
#define nn_s_submul1(a, b, m, c) \
   nn_s_submul1_c(a, b, m, c, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must be
   reduced mod d. The quotient q requires m limbs of space.  An 
   exception will result if d is 0.
*/
word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci);

/*
   Set q = a / d and return the remainder, where a is m words in 
   length and d is a word. The quotient q requires m limbs of space. 
   An exception will result if d is 0.
*/
#define nn_divrem1_simple(q, a, m, d) \
   nn_divrem1_simple_c(q, a, m, d, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must be
   reduced mod d. The quotient q requires m limbs of space.  An 
   exception will result if d is 0. Requires that d be normalised and
   that inv is a precomputed inverse of d computed by the function
   precompute_inverse1. If division by a non-normalised d is required,
   first normalise d, shift the carry-in by the same amount, then the
   remainder will also be shifted by the same amount upon return. No
   change to a or the quotient is required.
*/
word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t inv, word_t ci);

/* 
   As per _nn_divrem1_preinv_c but with no carry-in.
*/
#define nn_divrem1_preinv(q, a, m, d, inv) \
   nn_divrem1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Computes r and q such that r * B^m + a = q * d + ci, where
   q and a are m words in length, d is an odd word_t, inv is a
   precomputed Hensel inverse of d computed by the function
   precompute_hensel_inverse1 and ci is a "carry-in" word.
*/
word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci);

/*
   As per _nn_divrem_hensel1_preinv_c but with no "carry-in".
*/
#define nn_divrem_hensel1_preinv(q, a, m, d, inv) \
   nn_divrem_hensel1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Return a + ci * B^m mod d given a precomputed mod_preinv1_t,
   where ci is a "carry-in" word and a is m words in length, 
   that ci is reduced mod d and that d is not zero.
*/
word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci);

/* 
   Return a mod d given a precomputed mod_preinv1_t, where a is m 
   words in length and that d is not zero.
*/
#define nn_mod1_preinv(a, m, d, inv) \
   nn_mod1_preinv_c(a, m, d, inv, (word_t) 0)

/**********************************************************************
 
    Comparison

**********************************************************************/

/* 
   Return 1 if the m limbs at a match the n limbs at b,
   otherwise return 0.
*/
static inline
int nn_equal_m(nn_src_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      if (a[i] != b[i]) return 0;

   return 1;
}

/* 
   Return 1 if am == bm and the am limbs at a match the am 
   limbs at b, otherwise return 0.
*/
static inline
int nn_equal(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
      return 0;
   else 
      return nn_equal_m(a, b, am);
}

/* 
   Return a positive value if {a, m} > {b, m}, a negative
   value if {a, m} < {b, m}, otherwise returns 0.
*/
int nn_cmp_m(nn_src_t a, nn_src_t b, len_t m);

/* 
   Return a positive value if {a, am} > {b, bm}, a negative
   value if {a, am} < {b, bm}, otherwise returns 0.
*/
static inline
int nn_cmp(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
   {
      if (am > bm)
         return 1;
      else
         return -1;
   } else
      return nn_cmp_m(a, b, am);
}

/**********************************************************************
 
    Logical operations

**********************************************************************/

/* 
   Set b to the bitwise logical not of a.
*/
static inline
void nn_not(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = ~b[i];
}

/**********************************************************************
 
    Quadratic arithmetic functions

**********************************************************************/

/*
   Set {r, m1 + m2 - 1} = {a, m1} * {b, m2} and return any carry-out. 
   The output r may not alias either of the inputs a or b. We require 
   m1 >= m2 >= 0.
*/
word_t nn_mul_classical(nn_t r, nn_src_t a, len_t m1, nn_src_t b, len_t m2);

/*
   Set {r, m1 + m2 - 1} = {a, m1} * {b, m2} and write any carry-out. 
   Nothing is written if m1 is 0. The output r may not alias either of 
   the inputs a or b. We require m1 >= m2 >= 0.
*/
#define nn_s_mul_classical(r, a, m1, b, m2) \
   do { \
      if (m1) \
         r[m1 + m2 - 1] = nn_mul_classical(r, a, m1, b, m2); \
   } while (0)

#endif

