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

#ifndef BSD_ZZ0_H
#define BSD_ZZ0_H

#include "helper.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define ZZ0_SWAP(a, b)      \
   do {                     \
      nn_src_t __t = a;     \
      a = b;                \
      b = __t;              \
   } while (0)

#define ZZ0_ORDER(a, an, b, bn) \
   do {                         \
      if (an < bn) {            \
         BSDNT_SWAP(an, bn);    \
         ZZ0_SWAP(a, b);        \
      }                         \
   } while (0)

#define zz0_normalise(r, rsize)            \
   do {                                    \
      if (rsize < 0)                       \
         rsize = -nn_normalise(r, -rsize); \
      else                                 \
         rsize = nn_normalise(r, rsize);   \
   } while (0)

/**********************************************************************
 
    Arithmetic functions

**********************************************************************/

/*
   Set {r, ret} to {a, m} + {b, n}.
*/
len_t zz0_add(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   Set {r, ret} to {a, m} - {b, n}.
*/
len_t zz0_sub(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   Set {r, ret} to {a, m} - {b, n}.
*/
len_t zz0_mul(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n);

#ifdef __cplusplus
 }
#endif

#endif
