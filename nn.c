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

void nn_mul_m(nn_t p, nn_src_t a, nn_src_t b, len_t m)
{
   if (m <= MUL_CLASSICAL_CUTOFF)
      nn_mul_classical(p, a, m, b, m);
   else if (m <= MUL_KARA_CUTOFF)
      nn_mul_kara(p, a, m, b, m);
   else if (m <= MUL_TOOM33_CUTOFF)
      nn_mul_toom33(p, a, m, b, m);
   else
      talker("Out of options in nn_mul_m\n");
}

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
            } else
               talker("Out of options in nn_mul\n");
         } else /* n > 2*m3 */
         {
            if (n <= MUL_TOOM33_CUTOFF)
            {
               nn_mul_toom33(p, a, m, b, n);
               return;
            } else
               talker("Out of options in nn_mul\n");
         }
      }
   }

   r = m;

   while (r > n) r -= n;

   nn_mul(p, b, n, a, r);
  
   TMP_START;
   t = (nn_t) TMP_ALLOC(n + 1);
   while (m > r)
   {
      nn_copy(t, p + r, n); /* temporarily save top n + 1 limbs */
      nn_mul_m(p + r, a + r, b, n);
      nn_add(p + r, p + r, 2*n, t, n);
      r += n;
   }
   TMP_END;
}

void nn_mullow(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(m >= n);
   ASSERT(n > 0);
   
   if (m > n)
   {
      nn_t t;
      word_t ci;

      TMP_INIT;
    
      if (n >= m - n) nn_mul(p, b, n, a, m - n);
      else nn_mul(p, a, m - n, b, n);        
      
      TMP_START;

      t = (nn_t) TMP_ALLOC(n + 2);

      nn_mullow(t + n, t, a + m - n, n, b, n);
      
      /* do missing parts of mullow */

      ci = nn_add_m(p + m - n, p + m - n, t, n);
      nn_add1(ov, t + n, 2, ci);

      TMP_END;
   } else
      nn_mullow_m(ov, p, a, b, n);
}

void nn_divrem(nn_t q, nn_t a, len_t m, nn_src_t d, len_t n)
{   
   word_t norm, ci = 0;
   nn_t t;

   TMP_INIT;

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(a != d);
   ASSERT(n > 0);
   ASSERT(m >= n);
   
   TMP_START;
      
   if ((norm = high_zero_bits(d[n - 1])))
   {   
      t = (nn_t) TMP_ALLOC(n);
      ci = nn_shl(a, a, m, norm);
      nn_shl(t, d, n, norm);
   } else
      t = (nn_t) d;

   if (n == 1)
   {
      preinv1_t inv = precompute_inverse1(t[0]);
      a[0] = nn_divrem1_preinv_c(q, a, m, t[0], inv, ci);
   } else
   {
      preinv2_t inv = precompute_inverse2(t[n - 1], t[n - 2]);
      nn_divrem_divconquer_preinv_c(q, a, m, t, n, inv, ci);
   }

   if (norm)
      nn_shr(a, a, n, norm);

   TMP_END;
}

void nn_div(nn_t q, nn_t a, len_t m, nn_src_t d, len_t n)
{   
   word_t norm, ci = 0;
   nn_t t;

   TMP_INIT;

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(a != d);
   ASSERT(n > 0);
   ASSERT(m >= n);
   
   TMP_START;
      
   if ((norm = high_zero_bits(d[n - 1])))
   {   
      t = (nn_t) TMP_ALLOC(n);
      ci = nn_shl(a, a, m, norm);
      nn_shl(t, d, n, norm);
   } else
      t = (nn_t) d;

   if (n == 1)
   {
      preinv1_t inv = precompute_inverse1(t[0]);
      nn_divrem1_preinv_c(q, a, m, t[0], inv, ci);
   } else
   {
      preinv2_t inv = precompute_inverse2(t[n - 1], t[n - 2]);
      nn_div_divconquer_preinv_c(q, a, m, t, n, inv, ci);
   }

   TMP_END;
}
