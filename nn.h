#ifndef BSDNT_NN_H
#define BSDNT_NN_H

#include <stdint.h>
#include <stdlib.h>

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
 
    Linear arithmetic functions

**********************************************************************/

/*
   Set a = b + c + ci where b and c are both m words in length,
   ci is a "carry in". Return any carry out. 
*/
word_t _nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci);

/*
   Set a = b + c where b and c are both m words in length. Return 
   any carry out. 
*/
#define _nn_add_m(axxx, bxxx, cxxx, mxxx) \
   _nn_add_mc(axxx, bxxx, cxxx, mxxx, (word_t) 0)

/*
   Set a = b + c + ci where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_add_mc(axxx, bxxx, cxxx, mxxx, cixxx) \
   do { \
      if ((axxx) == (bxxx)) \
         (axxx)[mxxx] += _nn_add_mc(axxx, bxxx, cxxx, mxxx, cixxx); \
      else \
         (axxx)[mxxx] = _nn_add_mc(axxx, bxxx, cxxx, mxxx, cixxx); \
   } while (0)

/*
   Set a = b + c where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_add_m(axxx, bxxx, cxxx, mxxx) \
   nn_add_mc(axxx, bxxx, cxxx, mxxx, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length,
   bi is a "borrow". Return any borrow. 
*/
word_t _nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi);

/*
   Set a = b - c where b and c are both m words in length. Return 
   any borrow. 
*/
#define _nn_sub_m(axxx, bxxx, cxxx, mxxx) \
   _nn_sub_mc(axxx, bxxx, cxxx, mxxx, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the boorow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_sub_mc(axxx, bxxx, cxxx, mxxx, bixxx) \
   do { \
      if ((axxx) == (bxxx)) \
         (axxx)[mxxx] -= _nn_sub_mc(axxx, bxxx, cxxx, mxxx, bixxx); \
      else \
         (axxx)[mxxx] = -_nn_sub_mc(axxx, bxxx, cxxx, mxxx, bixxx); \
   } while (0)

/*
   Set a = b - c where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the borrow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_sub_m(axxx, bxxx, cxxx, mxxx) \
   nn_sub_mc(axxx, bxxx, cxxx, mxxx, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length,
   ci is a "carry in". Return any carry out. Assumes 0 <= bits
   < WORD_BITS.
*/
word_t _nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b << bits) where b is m words in length. Return 
   any carry out. Assumes 0 <= bits < WORD_BITS.
*/
#define _nn_shl(axxx, bxxx, mxxx, bitsxxx) \
   _nn_shl_c(axxx, bxxx, mxxx, bitsxxx, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length, ci is
   a "carry in", and writing the carry out to a[m]. Assumes 0 <= 
   bits < WORD_BITS.
*/
#define nn_shl_c(axxx, bxxx, mxxx, bitsxxx, cixxx) \
   do { \
      (axxx)[mxxx] = _nn_shl_c(axxx, bxxx, mxxx, bitsxxx, cixxx); \
   } while (0)

/*
   Set a = (b << bits) where b is m words in length, writing the 
   carry out to a[m]. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shl(axxx, bxxx, mxxx, bitsxxx) \
   nn_shl_c(axxx, bxxx, mxxx, bitsxxx, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, ci is a "carry in". Return any carry out from the low
   end. Assumes 0 <= bits < WORD_BITS.
*/
word_t _nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b >> bits) where b is m words in length. Return 
   any carry out from the low end. Assumes 0 <= bits < WORD_BITS.
*/
#define _nn_shr(axxx, bxxx, mxxx, bitsxxx) \
   _nn_shr_c(axxx, bxxx, mxxx, bitsxxx, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, and ci is a[m]*2^(WORD_BITS - bits). Assumes 0 <= bits < 
   WORD_BITS.
*/
#define nn_shr(axxx, bxxx, mxxx, bitsxxx) \
   do { \
      if (bitsxxx) \
         _nn_shr_c(axxx, bxxx, mxxx, bitsxxx, (bxxx)[mxxx] << (WORD_BITS - (bitsxxx))); \
      else \
         _nn_shr_c(axxx, bxxx, mxxx, bitsxxx, (word_t) 0); \
   } while (0)


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

#endif

