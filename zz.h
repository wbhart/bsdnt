/* 
  Copyright (C) 2013 William Hart

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

#ifndef BSDNT_ZZ_H
#define BSDNT_ZZ_H

#include <math.h>
#include <string.h> /* strspn */
#include <stdarg.h> /* variadic functions */
#include "helper.h"
#include "rand.h"
#include "nn.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
   nn_t n;
   len_t size;
   len_t alloc;
} zz_struct;

typedef zz_struct zz_t[1];

typedef zz_struct * zz_ptr;

typedef const zz_struct * zz_srcptr;

#define ZZ_SWAP(a, b)  \
  TYPED_SWAP(zz_ptr, a, b)

#define ZZ_SRC_SWAP(a, b)  \
  TYPED_SWAP(zz_srcptr, a, b)

#define ZZ_ORDER(a, an, b, bn)        \
   do {                               \
      if (an < bn) {                  \
         BSDNT_SWAP(an, bn);          \
         TYPED_SWAP(zz_srcptr, a, b); \
      }                               \
   } while (0)

#define TMP_ZZ(xxx) \
   do { \
      (xxx) = (zz_ptr) TMP_ALLOC_BYTES(sizeof(zz_t)); \
      zz_init(xxx); \
   } while (0)

/**********************************************************************
 
    Memory management

**********************************************************************/

/*
   Initialise a zz_t for use.
*/
void zz_init(zz_ptr r);

/*
   Initialize a null-terminated list of zz_ts for use.

   E.g.: given num1, num2 and num3, they can be initialized as
       zz_inits(num1, num2, num3, '\0');
*/
void zz_inits(zz_ptr num, ...);

/*
   Initialise a zz_t for use and ensure it has space for an integer
   whose absolute value has m words. This is included for performance
   reasons only. Usually it is sufficient to call zz_init.
*/
void zz_init_fit(zz_ptr r, len_t m);

/*
   Free any memory used by a zz_t.
*/
void zz_clear(zz_ptr r);

/*
   Free the memory used by every variable in a null-terminated list of
   zz_ts.

   E.g.: given num1, num2 and num3, they can be freed as
       zz_clears(num1, num2, num3, '\0');
*/
void zz_clears(zz_ptr num, ...);

/*
   Ensure a zz_t has space for an integer whose absolute value has m
   words. This is mainly used internally.
*/
void zz_fit(zz_ptr r, len_t m);

/*
   Normalise a zz_t so that its internal representation does not have
   any leading zero words. This function is mainly used internally.
*/
void zz_normalise(zz_ptr r);

/**********************************************************************
 
    Comparison

**********************************************************************/

/*
   Return 1 if the zz_t r is equal to the signed word c. Otherwise
   return 0.
*/
int zz_equali(zz_srcptr r, sword_t c);

/*
   Return 1 if the two zz_t's a and b are equal.
*/
static inline
int zz_equal(zz_srcptr a, zz_srcptr b)
{
   if (a->size != b->size)
      return 0;

   if (a->size == 0)
      return 1;

   return (nn_cmp_m(a->n, b->n, BSDNT_ABS(a->size)) == 0);
}

/*
   Compare the zz_t a with the signed word b. The return value
   will be positive if a > b, zero if a == b and negative if a < b.
*/
int zz_cmpi(zz_srcptr a, sword_t b);

/*
   Compare the zz_t's a and b. The return value will be positive if
   a > b, zero if a == b and negative if a < b.
*/
int zz_cmp(zz_srcptr a, zz_srcptr b);

/*
   Compare the absolute values of the zz_t's a and b. The return value
   will be positive if abs(a) > abs(b), zero if abs(a) == abs(b) and
   negative if abs(a) < abs(b).
*/
int zz_cmpabs(zz_srcptr a, zz_srcptr b);

/*
   Return 1 if the zz_t is zero, otherwise return 0.
*/
int zz_is_zero(zz_srcptr r);

/**********************************************************************
 
    Random generation

**********************************************************************/

/*
   Generate a signed, uniformly random zz_t whose absolute value has
   at most the given number of words.
*/
void zz_random(zz_ptr a, rand_t state, len_t words);

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

/*
   Set the zz_t to the given signed word c.
*/
void zz_seti(zz_ptr r, sword_t c);

/*
   Set r = a. The data is copied unless a and b are the same object.
*/
void zz_set(zz_ptr r, zz_srcptr a);

/*
   Set r = -a. The date is copied unless r and a are the same object,
   in which case its sign is simply changed.
*/
void zz_neg(zz_ptr r, zz_srcptr a);

/*
   Efficiently swap the contents of a and b by swapping pointers and
   other information.
*/
void zz_swap(zz_ptr a, zz_ptr b);

/*
   Set the zz_t to zero.
*/
void zz_zero(zz_ptr a);

/**********************************************************************
 
    Basic arithmetic

**********************************************************************/

/*
   Set r = a + c, where c is a signed word.
*/
void zz_addi(zz_ptr r, zz_srcptr a, sword_t c);

/*
   Set r = a - c, where c is a signed word.
*/
void zz_subi(zz_ptr r, zz_srcptr a, sword_t c);

/*
   Set r = a + b.
*/
void zz_add(zz_ptr r, zz_srcptr a, zz_srcptr b);

/*
   Set r = a - b.
*/
void zz_sub(zz_ptr r, zz_srcptr a, zz_srcptr b);

/*
   Set r = a*2^exp.
*/
void zz_mul_2exp(zz_ptr r, zz_srcptr a, bits_t exp);

/*
   Set r = a/2^exp.
*/
void zz_div_2exp(zz_ptr r, zz_srcptr a, bits_t exp);

/*
   Set r = a*c, where c is a signed word.
*/
void zz_muli(zz_ptr r, zz_srcptr a, sword_t c);

/*
   Set q such that a = bq + r with |r| < |b|. Rounding occurs towards
   minus infinity. The remainder is returned as a signed word.
*/
sword_t zz_divremi(zz_ptr q, zz_srcptr a, sword_t b);

/*
   Set r = a*b.
*/
void zz_mul(zz_ptr r, zz_srcptr a, zz_srcptr b);

/*
   Set q and r such that a = bq + r with |r| < |b|. Rounding occurs
   towards minus infinity.
*/
void zz_divrem(zz_ptr q, zz_ptr r, zz_srcptr a, zz_srcptr b);

/*
   Set q such that a = bq + r with |r| < |b|. Rounding occurs towards
   minus infinity.
*/
void zz_div(zz_ptr q, zz_srcptr a, zz_srcptr b);

/*
   Set r = a ^ b.
   b must be non-negative.
*/
void zz_powi(zz_ptr r, zz_srcptr a, sword_t b);

/*
   Set g to the greatest common divisor of a and b. If both are
   negative or zero then g will be negative or zero, otherwise g will
   be positive. If a = 0 then g = b, and similarly if b = 0 then g = a.
*/
void zz_gcd(zz_ptr g, zz_srcptr a, zz_srcptr b);

/*
   As for zz_gcd except that additionally s and t are returned with
   |s| < |b| and |t| < |a| and g = as + bt.
*/
void zz_xgcd(zz_ptr g, zz_ptr s, zz_ptr t, zz_srcptr a, zz_srcptr b);

/**********************************************************************
 
    I/O

**********************************************************************/

/*
   Return a string giving the decimal representation of the zz_t a.
   If the value is negative, the string is prepended with a '-' sign.
   If the value is 0, the string "0" is returned. The user is
   required to clean up the string using free when finished.
*/
char * zz_get_str(zz_srcptr a);

/*
   Set the zz_t a to the integer whose decimal representation is
   given by the string str. If a negative value is required, the
   string can be prepended with a '-' character.
*/
size_t zz_set_str(zz_t a, const char * str);

/*
   Print the decimal representation of the zz_t a to stdout. No
   newline character is output.
*/
void zz_print(zz_srcptr a);

#ifdef __cplusplus
 }
#endif

#endif

