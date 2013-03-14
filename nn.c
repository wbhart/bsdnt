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
#include "nn_arch.h"
#include "tuning.h"

void nn_mul(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t r;
   nn_t t;
   TMP_INIT;
   
   if (n <= MUL_CLASSICAL_CUTOFF)
   {
      nn_mul_classical(p, a, m, b, n);
      return;
   } 
   
   if (n <= MUL_KARA_CUTOFF)
   {
      if (n > (m + 1)/2)
      {
         nn_mul_kara(p, a, m, b, n);
         return;
      }
   } else /* too big for Karatsuba */
   {
      const len_t m3 = (m + 2)/3;
      
      if (n > m3)
      {
         if (n <= 2*m3)
         {
            if (n <= MUL_TOOM32_CUTOFF)
            {
               nn_mul_toom32(p, a, m, b, n);
               return;
            }
         } else /* n > 2*m3 */
         {
            if (n <= MUL_TOOM33_CUTOFF)
            {
               nn_mul_toom33(p, a, m, b, n);
               return;
            }
         }
      }
   }

   r = m;

   while (r > n) r -= n;

   nn_mul_classical(p, b, n, a, r);
  
   if (m > r)
   {
      TMP_START;
      t = TMP_ALLOC(n + 1);
      while (m > r)
      {
         nn_copy(t, p + r, n); /* temporarily save top n + 1 limbs */
         nn_mul_classical(p + r, a + r, n, b, n);
         nn_add(p + r, p + r, 2*n, t, n);
         r += n;
      }
      TMP_END;
   }
}

