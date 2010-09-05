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

static inline
nn_t nn_init(len_t m)
{
   if (m) return malloc(m*sizeof(word_t));
   else return NULL;
}

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
      if (axxx == bxxx) \
         axxx[mxxx] += _nn_add_mc(axxx, bxxx, cxxx, mxxx, cixxx); \
      else \
         axxx[mxxx] = _nn_add_mc(axxx, bxxx, cxxx, mxxx, cixxx); \
   } while (0)

/*
   Set a = b + c where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_add_m(axxx, bxxx, cxxx, mxxx) \
   nn_add_mc(axxx, bxxx, cxxx, mxxx, (word_t) 0)

/**********************************************************************
 
    Comparison

**********************************************************************/

/* 
   Return 1 if the m limbs at a match the n limbs at b,
   otherwise return 0.
*/
static inline
int nn_equal(nn_src_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      if (a[i] != b[i]) return 0;

   return 1;
}

#endif

