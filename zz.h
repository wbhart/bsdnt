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
#include "helper.h"
#include "rand.h"
#include "nn.h"

typedef struct
{
   nn_t n;
   len_t size;
   len_t alloc;
} zz_struct;

typedef zz_struct zz_t[1];

typedef zz_struct * zz_ptr;

typedef const zz_struct * zz_srcptr;

#define ZZ_SWAP(a, b)      \
   do {                    \
      zz_srcptr __t = a;   \
      a = b;               \
      b = __t;             \
   } while (0)

#define ZZ_ORDER(a, an, b, bn) \
   do {                        \
      if (an < bn) {           \
         BSDNT_SWAP(an, bn);   \
         ZZ_SWAP(a, b);        \
      }                        \
   } while (0)

/**********************************************************************
 
    Memory management

**********************************************************************/

void zz_init(zz_ptr r);

void zz_init_fit(zz_ptr r, len_t m);

void zz_clear(zz_ptr r);

void zz_fit(zz_ptr r, len_t m);

void zz_normalise(zz_ptr r);

/**********************************************************************
 
    Comparison

**********************************************************************/

int zz_equali(zz_srcptr r, sword_t c);

static inline
int zz_equal(zz_srcptr a, zz_srcptr b)
{
   if (a->size != b->size)
      return 0;

   if (a->size == 0)
      return 1;

   return (nn_cmp_m(a->n, b->n, BSDNT_ABS(a->size)) == 0);
}

int zz_cmpi(zz_srcptr a, sword_t b);

int zz_cmp(zz_srcptr a, zz_srcptr b);

int zz_cmpabs(zz_srcptr a, zz_srcptr b);

int zz_is_zero(zz_srcptr r);

/**********************************************************************
 
    Random generation

**********************************************************************/

void zz_random(zz_ptr a, rand_t state, bits_t bits);

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

void zz_seti(zz_ptr r, sword_t c);

void zz_set(zz_ptr a, zz_srcptr b);

void zz_neg(zz_ptr r, zz_srcptr a);

void zz_swap(zz_ptr a, zz_ptr b);

void zz_zero(zz_ptr a);

/**********************************************************************
 
    Basic arithmetic

**********************************************************************/

void zz_addi(zz_ptr r, zz_srcptr a, sword_t c);

void zz_subi(zz_ptr r, zz_srcptr a, sword_t c);

void zz_add(zz_ptr r, zz_srcptr a, zz_srcptr b);

void zz_sub(zz_ptr r, zz_srcptr a, zz_srcptr b);

void zz_mul_2exp(zz_ptr r, zz_srcptr a, bits_t exp);

void zz_div_2exp(zz_ptr r, zz_srcptr a, bits_t exp);

void zz_muli(zz_ptr r, zz_srcptr a, sword_t c);

sword_t zz_divremi(zz_ptr q, zz_srcptr a, sword_t b);

void zz_mul(zz_ptr r, zz_srcptr a, zz_srcptr b);

void zz_divrem(zz_ptr q, zz_ptr r, zz_srcptr a, zz_srcptr b);

void zz_div(zz_ptr q, zz_srcptr a, zz_srcptr b);

void zz_gcd(zz_ptr g, zz_srcptr a, zz_srcptr b);

void zz_xgcd(zz_ptr g, zz_ptr s, zz_ptr t, zz_srcptr a, zz_srcptr b);

/**********************************************************************
 
    I/O

**********************************************************************/

char * zz_get_str(zz_srcptr a);

size_t zz_set_str(zz_t a, const char * str);



#endif

