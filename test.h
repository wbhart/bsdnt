#ifndef BSDNT_GENERIC_H
#define BSDNT_GENERIC_H

#include "nn.h"

#define TEST_START(XXX, YYY) \
   do { \
      long __count; \
      const char * __test_name = #XXX; \
      for (__count = 0; __count < YYY  && result == 1; __count++)

#define TEST_END \
      if (!result && __count) printf("Test %s, iteration %ld...", __test_name, __count); \
      gc_cleanup(); \
   } while (0)

#define RUN(xxx) \
   do { \
      if (xxx()) \
      { \
         printf("PASS\n"); \
         pass++; \
      } else \
      { \
         printf("FAIL\n"); \
         fail++; \
      } \
   } while (0)

#define print_debug(a, m) \
   do { \
      printf(#a "(%ld) = ", m); nn_printx_short(a, m); printf("\n"); \
   } while (0)

#define print_debug_diff(a, b, m) \
   do { \
   printf(#a " vs " #b "(%ld) ", m); nn_printx_diff(a, b, m); printf("\n"); \
   } while (0)

typedef enum
{
   NN 
} type_t;

typedef enum
{
   ANY, NONZERO, ODD, FULL
} flag_t;

typedef struct node_t
{
   type_t type;
   void * ptr;
   struct node_t * next;
} node_t;

/*
   Send the given error message to stderr.
*/
void talker(const char * str);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag.

   randoms(NONZERO, state, &w1, &w2, NULL);
*/
void randoms(flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag and
   which are below the given limit.

   randoms_upto(100, ANY, state, &w1, &w2, NULL);
*/
void randoms_upto(word_t limit, flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to nn_t's, generate 
   random integers which have the property specified by the flag and
   which have the specified number of limbs. These can only be
   cleaned up by calling gc_cleanup() (see below).

   randoms_upto(100, FULL, state, &a, &b, NULL);
*/
void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

/*
   Clean up any objects allocated by the random functions above.
   Note *all* objects are cleaned up. We have no way of knowing 
   which are still in use, so it is assumed none of them are still
   in use.
*/
void gc_cleanup(void);

#endif

