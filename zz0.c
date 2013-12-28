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

#include "nn.h"
#include "zz0.h"

len_t zz0_add(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t asize = BSDNT_ABS(m);
   len_t bsize = BSDNT_ABS(n);
   len_t rsize;
   len_t big = asize >= bsize ? m : n;
   
   ZZ0_ORDER(a, asize, b, bsize);
   
   if ((m ^ n) < 0) 
   {
      word_t bi = nn_sub(r, a, asize, b, bsize);
      rsize = asize;
      
      if (bi) {
         nn_neg(r, r, asize);
         rsize = -rsize;
      }
   } else 
   {
      r[asize] = nn_add(r, a, asize, b, bsize);
      rsize = asize + 1;
   }

   if (big < 0)
      rsize = -rsize;

   zz0_normalise(r, rsize);

   return rsize;
}

len_t zz0_sub(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t asize = BSDNT_ABS(m);
   len_t bsize = BSDNT_ABS(n);
   len_t rsize;
   len_t sign = asize - bsize;

   ZZ0_ORDER(a, asize, b, bsize);
   
   if ((m ^ n) >= 0) {
      word_t bi = nn_sub(r, a, asize, b, bsize);
      rsize = asize;
      
      if (bi) {
         nn_neg(r, r, asize);
         rsize = -rsize;
      }
      if ((sign ^ m) < 0) rsize = -rsize;
   } else {
      r[asize] = nn_add(r, a, asize, b, bsize);
      rsize = asize + 1;
      if (m < 0) rsize = -rsize;
   }

   zz0_normalise(r, rsize);

   return rsize;
}

len_t zz0_mul(nn_t r, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   long asize = BSDNT_ABS(m);
   long bsize = BSDNT_ABS(n);
   long rsize = asize + bsize;
   
   if (asize == 0 || bsize == 0)
      return 0;
   else {
      ZZ0_ORDER(a, asize, b, bsize);

      nn_mul(r, a, asize, b, bsize);
      rsize -= (r[rsize - 1] == 0);

      return (m ^ n) < 0 ? -rsize : rsize;
   }
}
