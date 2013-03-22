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
#include "nn_quadratic_arch.h"

#ifndef HAVE_ARCH_nn_mul_classical

void nn_mul_classical(nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2)
{
   len_t i;
   
   ASSERT(r != a);
   ASSERT(r != b);
   ASSERT(m1 > 0);
   ASSERT(m2 > 0);

   r[m1] = nn_mul1(r, a, m1, b[0]); 
   
   for (i = 1; i < m2; i++)
      r[m1 + i] = nn_addmul1(r + i, a, m1, b[i]);
}

#endif

#ifndef HAVE_ARCH_nn_mullow_classical

void nn_mullow_classical(nn_t ov, nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2)
{
   len_t i;
   dword_t t;
  
   ASSERT(r != a);
   ASSERT(r != b);
   ASSERT(m1 >= m2);
   ASSERT(m2 > 0);

   t = (dword_t) nn_mul1(r, a, m1, b[0]); 
   
   for (i = 1; i < m2; i++)
      t += (dword_t) nn_addmul1(r + i, a, m1 - i, b[i]);

   ov[0] = (word_t) t;
   ov[1] = (word_t) (t >> WORD_BITS);
}

#endif

#ifndef HAVE_ARCH_nn_mulhigh_classical

void nn_mulhigh_classical(nn_t r, nn_src_t a, len_t m1, 
                                       nn_src_t b, len_t m2, nn_t ov)
{
   len_t i;
   dword_t t;
   
   ASSERT(r != a);
   ASSERT(r != b);
   ASSERT(m1 >= m2);
   ASSERT(m2 > 0);

   if (m2 == 1) /* overflow is one limb in this case */
   {
      r[0] = ov[0];
      return;
   }

   /* a[m1 - 1] * b[1] + ov[0] */
   t = (dword_t) a[m1 - 1] * (dword_t) b[1] + (dword_t) ov[0];
   r[0] = (word_t) t;
   
   if (m2 > 2)
   {
      /* {a[m1 - 2], a[m1 - 1]} * b[2] + ov[1] */
      r[1] = (t >> WORD_BITS);
      r[2] = nn_addmul1(r, a + m1 - 2, 2, b[2]);
      t = (dword_t) ov[1] + (dword_t) r[1];
      r[1] = (word_t) t;
      t = (t >> WORD_BITS) + (dword_t) r[2];
	   r[2] = (word_t) t; /* possible overflow */
   } else
      r[1] = (t >> WORD_BITS) + ov[1]; /* ov[1] cannot be more than 1 in this case */

   for (i = 3; i < m2; i++)
      r[i] = nn_addmul1(r, a + m1 - i, i, b[i]);

   /* deal with overflow */
   if (m2 > 3) 
      r[m2 - 1] += nn_add1(r + 3, r + 3, m2 - 4, (word_t) (t >> WORD_BITS));
}

#endif

#ifndef HAVE_ARCH_nn_mulmid_classical

void nn_mulmid_classical(nn_t ov, nn_t p,
                            nn_src_t a, len_t m, nn_src_t b, len_t n)
{
  dword_t t; /* overflow */
 
  ASSERT(m >= n);
  ASSERT(n >= 2);

  a += n;
  m -= n;

  t = nn_mul1(p, a, m, b[0]);

  for (n--; n > 0; n--)
  {
      a--; b++;
      t += nn_addmul1(p, a, m, b[0]);
  }

  ov[0] = (word_t) t;
  ov[1] = (t >> WORD_BITS);
}

#endif

/* GCC thinks d1 is unused, so turn warning off */
#pragma GCC diagnostic ignored "-Wunused-variable"

#ifndef HAVE_ARCH_nn_divrem_classical_preinv_c

void nn_divrem_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_t dinv, word_t ci)
{
   long i = m - 1, j = m - n;
   word_t d1 = d[n - 1];

   ASSERT(q != d);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((ci < d1) 
      || ((ci == d1) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((long) d1 < 0);

   for ( ; i >= n - 1; i--, j--)
   {
      divapprox21_preinv1(q[j], ci, a[i], d1, dinv);
      
	  /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q[j]);
      
      /* correct if remainder is too large */
      while (ci || nn_cmp_m(a + j, d, n) >= 0)
      {
         q[j]++;
         ci -= nn_sub_m(a + j, a + j, d, n);
      }
      
      /* fetch next word now that it has been updated */
      ci = a[i];
   }
}


#endif

/* Turn warning back on */
#pragma GCC diagnostic warning "-Wunused-variable"

#ifndef HAVE_ARCH_nn_divapprox_classical_preinv_c

word_t nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_t dinv, word_t ci)
{
   long i = m - 1, j = m - n;
   word_t cy, d1 = d[n - 1];
   len_t s = m - n; /* this many iterations to get to last quotient */
   s += 2; /* need two normalised words at that point */
   
   ASSERT(q != d);
   ASSERT(q != a);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((long) d1 < 0);

   for ( ; s >= n; i--, j--, s--)
   {
      divapprox21_preinv1(q[j], ci, a[i], d1, dinv);

      /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q[j]);
      
      /* correct if remainder is too large */
      while (ci || nn_cmp_m(a + j, d, n) >= 0)
      {
         q[j]++;
         ci -= nn_sub_m(a + j, a + j, d, n);
      }
	  
      /* fetch next word now that it has been updated */
      ci = a[i];
   }
   
   d = d + n - s; /* make d length s */
   a = a + i + 1 - s; /* make a length s (+ carry) */
   
   for ( ; s >= 2; j--, s--)
   {
      /* rare case where truncation ruins normalisation */
      if (ci > d[s - 1] || (ci == d[s - 1] && nn_cmp_m(a + 1, d, s - 1) >= 0))
      {
         for ( ; s >= 2; j--, s--)
         {
            q[j] = ~WORD(0);
            ci -= nn_submul1(a, d, s, q[j]);
            cy = ci;
            ci = a[s - 1];
            d++;
         }

         return cy;
      }

      divapprox21_preinv1(q[j], ci, a[s - 1], d1, dinv);
         
      /* a -= d*q1 */
      ci -= nn_submul1(a, d, s, q[j]);

	   /* correct if remainder is too large */
      while (ci || nn_cmp_m(a, d, s) >= 0)
      {
         q[j]++;
         ci -= nn_sub_m(a, a, d, s);
      }

      /* fetch next word now that it has been updated */
      cy = ci;
      ci = a[s - 1];
      
      d++;
   }  

   return cy;
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

   t = (dword_t) ct;

   for ( ; i < m; i++)
   {
      q[i] = a[i] * inv;
      t += (dword_t) nn_submul1(a + i, d, m - i, q[i]);
   }
   
   ov[0] = (word_t) t;
   ov[1] = (t >> WORD_BITS);
}

#endif
