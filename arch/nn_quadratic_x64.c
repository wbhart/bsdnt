/* 
  Copyright (C) 2010, William Hart
  Copyright (C) 2010, Brian Gladman

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

#ifndef HAVE_ARCH_nn_divrem_classical_preinv_c

void nn_divrem_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_2_t inv, word_t ci)
{
   dword_t t;
   long i, j = m - n;
   word_t q1, rem;
   word_t norm = inv.norm;
   word_t dinv = inv.dinv;
   word_t d1 = inv.d1;

   for (i = m - 1; i >= n - 1; i--, j--)
   {
     if(norm)
     {
         t.hi = (ci << norm) | (a[i] >> (WORD_BITS - norm));
         t.lo = a[i] << norm;
     }
     else
     {
         t.hi = ci;
         t.lo = a[i];
     }
     if(t.hi == d1)
          q1 = ~(word_t) 0;
      else 
          divrem21_preinv1(q1, rem, t, d1, dinv);

      /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q1);
      
      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a + j, a + j, d, n);
      }

      q[j] = q1;
      ci = a[i];
   }
}

#endif

#ifndef HAVE_ARCH_nn_divapprox_classical_preinv_c

void nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_2_t inv, word_t ci)
{
   dword_t t;
   long i = m - 1, j = m - n;
   word_t q1, rem;
   word_t norm = inv.norm;
   word_t dinv = inv.dinv;
   word_t d1 = inv.d1;
   len_t s = m - n; /* this many iterations to get to last quotient */
   s = 2 + s + (d[n-1] <= 2*s); /* need two normalised words at that point */
   if (s > i + 1) s = i + 1; /* ensure we don't do too many iterations */
   
   for ( ; s >= n; i--, j--, s--)
   {
      /* top "two words" of remaining dividend, shifted */
     if(norm)
     {
         t.hi = (ci << norm) | (a[i] >> (WORD_BITS - norm));
         t.lo = a[i] << norm;
     }
     else
     {
         t.hi = ci;
         t.lo = a[i];
     }
      
      /* check for special case, a1 == d1 which would cause overflow */
     if(t.hi == d1)
          q1 = ~(word_t) 0;
      else 
          divrem21_preinv1(q1, rem, t, d1, dinv);
      /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q1);
      
      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a + j, a + j, d, n);
      }

      q[j] = q1;
      ci = a[i];
   }
   
   d = d + n - s; /* make d length s */
   a = a + i + 1 - s; /* make a length s + carry */
   
   for ( ; i >= n - 1; i--, j--, s--)
   {
      /* top "two words" of remaining dividend, shifted */
     if(norm)
     {
         t.hi = (ci << norm) | (a[s - 1] >> (WORD_BITS - norm));
         t.lo = a[s - 1] << norm;
     }
     else
     {
         t.hi = ci;
         t.lo = a[s - 1];
     }

      /* check for special case, a1 == d1 which would cause overflow */
     if(t.hi == d1)
          q1 = ~(word_t) 0;
      else 
          divrem21_preinv1(q1, rem, t, d1, dinv);
      /* a -= d*q1 */
      ci -= nn_submul1(a, d, s, q1);
 
      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a, a, d, s);
      }

      q[j] = q1;
      ci = a[s - 1];
      d++;
   }   
}

#endif
