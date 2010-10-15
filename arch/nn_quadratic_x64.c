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

#ifndef HAVE_ARCH_nn_mullow_classical

void nn_mullow_classical(nn_t ov, nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2)
{
   len_t i;
   dword_t t;
  
   t.lo = nn_mul1(r, a, m1, b[0]); 
   t.hi = 0;

   for (i = 1; i < m2; i++)
   {
       word_t v = nn_addmul1(r + i, a, m1 - i, b[i]);
       t.hi += ((t.lo += v) < v ? 1 : 0);
   }
   ov[0] = t.lo;
   ov[1] = t.hi;
}

#endif

#ifndef HAVE_ARCH_nn_mulhigh_classical

word_t nn_mulhigh_classical(nn_t r, nn_src_t a, len_t m1, 
                                       nn_src_t b, len_t m2, nn_t ov)
{
   len_t i;
   word_t t, ci;

   if (m2 == 1)
      return ov[0]; /* overflow is one limb in this case */

   /* a[m1 - 1] * b[1] + ov[0]*/
   t = mul_64_by_64(a[m1 - 1], b[1], &ci);
   ci += ((r[0] = t + ov[0]) < t ? 1 : 0);
   
   if (m2 > 2)
   {
      /* {a[m1 - 2], a[m1 - 1]} * b[2] + ov[1] */
      r[1] = ci;
      ci = nn_addmul1(r, a + m1 - 2, 2, b[2]);

      ci += ((t = r[1] + ov[1]) < r[1] ? 1 : 0);
      r[1] = t;
   } 
   else
      ci += ov[1]; /* ov[1] cannot be more than 1 in this case */

   for (i = 3; i < m2; i++)
   {
      r[i - 1] = ci;
      ci = nn_addmul1(r, a + m1 - i, i, b[i]);
   }

   return ci;
}

#endif

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
     t.hi = (ci << norm) | (norm ? a[i] >> (WORD_BITS - norm) : 0);
     t.lo = a[i] << norm; 

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
      t.hi = (ci << norm) | (norm ? a[i] >> (WORD_BITS - norm) : 0);
      t.lo = a[i] << norm; 
      
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
      t.hi = (ci << norm) | (norm ? a[s - 1] >> (WORD_BITS - norm) : 0);
      t.lo = a[s - 1] << norm; 

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

#ifndef HAVE_ARCH_nn_div_hensel_preinv

void nn_div_hensel_preinv(nn_t ov, nn_t q, nn_t a, len_t m, 
                            nn_src_t d, len_t n, hensel_preinv1_t inv)
{
   long i;
   dword_t t;
   word_t ci, ct = 0;
   
   ASSERT(q != d);
   ASSERT(q != a);
   ASSERT(m >= n);
   ASSERT(n > 0);
   ASSERT(d[0] & 1);

   for (i = 0; i < m - n; i++)
   {
      q[i] = a[i] * inv;
      ci = nn_submul1(a + i, d, n, q[i]);
      ct += nn_sub1(a + i + n, a + i + n, m - i - n, ci);
   }

   t.lo = ct;
   t.hi = 0;

   for ( ; i < m; i++)
   {
      q[i] = a[i] * inv;
      ct = nn_submul1(a + i, d, m - i, q[i]);
      t.hi += ((t.lo += ct) < ct ? 1 : 0);
   }
   
   ov[0] = t.lo;
   ov[1] = t.hi;
}

#endif
