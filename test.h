/* 
  Copyright (C) 2010, 2013 William Hart

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

#ifndef BSDNT_TEST_H
#define BSDNT_TEST_H

#include "nn.h"
#include "zz.h"
#include "config.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if WANT_REDZONES
#define REDZONE_WORDS 4 /* number of words of redzone */
#else
#define REDZONE_WORDS 0
#endif

#define REDZONE_BYTE 0xA /* byte of data to use in a redzone */

#define checkpoint_rand(s) \
   do { \
      printf(s); \
      bsdnt_printf("%wx\n", randword(state)); \
   } while (0)

#define TEST_START(XXX, YYY) \
   do { \
      long __count; \
      const char * __test_name = #XXX; \
      for (__count = 0; __count < YYY  && result == 1; __count++)

#define TEST_END \
      if (!result && __count) printf("Test %s, iteration %ld...", __test_name, __count); \
      gc_cleanup(); \
   } while (0)

#define test_zz_aliasing(xxx, yyy, s, t, tz) \
   do { \
      int __resa; \
      xxx; \
      zz_set(tz, t); \
      yyy; \
      __resa = zz_equal(s, tz); \
      if (!__resa) printf("Aliasing of " #s " and " #t " failed\n"); \
      result &= __resa; \
   } while (0)

#define test_zz_aliasing_12(xxx, a, b, c) \
   do { \
      zz_t __tzz; \
      zz_init(__tzz); \
      test_zz_aliasing(xxx(a, b, c), xxx(__tzz, __tzz, c), a, b, __tzz); \
      test_zz_aliasing(xxx(a, b, c), xxx(__tzz, b, __tzz), a, c, __tzz); \
      zz_clear(__tzz); \
   } while (0)

#define test_zz_aliasing_22(xxx, a, b, c, d) \
   do { \
      zz_t __tzz; \
      zz_init(__tzz); \
      test_zz_aliasing(xxx(a, b, c, d), xxx(__tzz, b, __tzz, d), a, c, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d), xxx(__tzz, b, c, __tzz), a, d, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d), xxx(a, __tzz, __tzz, d), b, c, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d), xxx(a, __tzz, c, __tzz), b, d, __tzz); \
      zz_clear(__tzz); \
   } while (0)

#define test_zz_aliasing_32(xxx, a, b, c, d, e) \
   do { \
      zz_t __tzz; \
      zz_init(__tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(__tzz, b, c, d, __tzz), a, e, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(__tzz, b, c, __tzz, e), a, d, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(a, __tzz, c, d, __tzz), b, e, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(a, __tzz, c, __tzz, e), b, d, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(a, b, __tzz, d, __tzz), c, e, __tzz); \
      test_zz_aliasing(xxx(a, b, c, d, e), xxx(a, b, __tzz, __tzz, e), c, d, __tzz); \
      zz_clear(__tzz); \
   } while (0)

#define test_zz_aliasing_12i(xxx, a, b, c) \
   do { \
      zz_t __tzz; \
      zz_init(__tzz); \
      test_zz_aliasing(xxx(a, b, c), xxx(__tzz, __tzz, c), a, b, __tzz); \
      zz_clear(__tzz); \
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
      bsdnt_printf(#a "(%m) = ", m); \
      nn_printx_short(a, BSDNT_ABS(m)); printf("\n"); \
   } while (0)

#define print_debug_diff(a, b, m) \
   do { \
   bsdnt_printf(#a " vs " #b "(%m) ", m); nn_printx_diff(a, b, BSDNT_ABS(m)); printf("\n"); \
   } while (0)

#define zz_print_debug(a) \
   do { \
      bsdnt_printf(#a "(%m) = ", a->size); \
      zz_print(a); printf("\n"); \
   } while (0)

typedef enum
{
   NN, ZZ
} type_t;

typedef enum
{
   ANY, NONZERO, ODD, FULL, NORMALISED, POSITIVE
} flag_t;

typedef struct node_t
{
   type_t type;
   void * ptr;
   len_t length;
   struct node_t * next;
} node_t;

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
   which have the specified number of words. These can only be
   cleaned up by calling gc_cleanup() (see below).

   randoms_of_len(100, FULL, state, &a, &b, NULL);
*/
void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of zz_t's, generate random signed integers
   which have the property specified by the flag and which have the
   specified number of words. These can only be cleaned up by calling
   gc_cleanup() (see below).

   randoms_signed(100, FULL, state, &a, &b, NULL);
*/
void randoms_signed(len_t n, flag_t flag, rand_t state, ...);

/*
   Clean up any objects allocated by the random functions above.
   Note *all* objects are cleaned up. We have no way of knowing 
   which are still in use, so it is assumed none of them are still
   in use.
*/
void gc_cleanup(void);

#ifdef __cplusplus
 }
#endif

#endif

