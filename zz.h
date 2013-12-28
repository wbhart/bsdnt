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

#include "helper.h"

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

/**********************************************************************
 
    Comparison

**********************************************************************/

static inline
int zz_equal(zz_srcptr a, zz_srcptr b)
{
   if (a->size != b->size)
      return 0;

   if (a->size == 0)
      return 1;

   return (nn_cmp_m(a->n, b->n, BSDNT_ABS(a->size)) == 0);
}

#endif

