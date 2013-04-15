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

#define _NN_MULMID_RFIX_M(_op_) \
   len_t i; \
   dword_t s, t = 0; \
   word_t ci = 0; \
   ASSERT(n >= 2); \
   a += n - 2; \
   nn_zero(p, n); \
   for (i = 0; i < n - 1; i++, a--) \
   { \
      s = (dword_t) b1[i] _op_ (dword_t) b2[i] _op_ (dword_t) ci; \
      r[i] = (word_t) s; \
      if ((ci = _op_ (s >> WORD_BITS))) \
         t += (dword_t) a[n] - (dword_t) nn_sub1(p, p, n, a[0]); \
   } \
   s = (dword_t) b1[i] _op_ (dword_t) b2[i] _op_ (dword_t) ci; \
   r[i] = (word_t) s; \
   if ((ci = _op_ (s >> WORD_BITS))) \
      t += (dword_t) a[n] + (dword_t) nn_add_m(p + 1, p + 1, a + 1, n - 1); \
   ov[0] = (word_t) t; \
   ov[1] = (t >> WORD_BITS); \
   return ci;

#ifndef HAVE_ARCH__nn_mulmid_add_rfix_m

word_t _nn_mulmid_add_rfix_m(nn_t r, nn_t ov, nn_t p,
                       nn_src_t a, nn_src_t b1, nn_src_t b2, len_t n)
{
   _NN_MULMID_RFIX_M(+);
}

#endif

#ifndef HAVE_ARCH__nn_mulmid_sub_rfix_m

word_t _nn_mulmid_sub_rfix_m(nn_t r, nn_t ov, nn_t p,
                       nn_src_t a, nn_src_t b1, nn_src_t b2, len_t n)
{
   _NN_MULMID_RFIX_M(-);
}

#endif

#ifndef HAVE_ARCH__nn_mulmid_add_lfix

word_t _nn_mulmid_add_lfix_m(nn_t r, nn_t ov, nn_t p,
                    nn_src_t a1, nn_src_t a2, nn_src_t b, len_t n)
{
   len_t i;
   dword_t s, t = 0;
   word_t ci = 0;

   ASSERT(n >= 2);

   b += n - 1;

   nn_zero(p, n);

   for (i = 0; i < n; i++, b--)
   {
      if (ci) t -= (dword_t) nn_sub1(p, p, n, b[0]);
      s = (dword_t) a1[i] + (dword_t) a2[i] + (dword_t) ci;
      r[i] = (word_t) s;
      ci = (s >> WORD_BITS);  
   }

   for ( ; i < 2*n - 1; i++, b--)
   {
       if (ci) t += (dword_t) b[n];
       s = (dword_t) a1[i] + (dword_t) a2[i] + (dword_t) ci;
       r[i] = (word_t) s;
       ci = (s >> WORD_BITS);
   }

   if (ci) t += (dword_t) b[n];

   ov[0] = (word_t) t;
   ov[1] = (t >> WORD_BITS);

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mulmid_classical

void nn_mulmid_classical(nn_t ov, nn_t p,
                            nn_src_t a, len_t m, nn_src_t b, len_t n)
{
  dword_t t; /* overflow */
 
  ASSERT(m + 1 >= n);
  ASSERT(n >= 2);

  a += n - 1;
  m -= n - 1;

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
                                  len_t n, preinv2_t dinv, word_t ci)
{
   long j;
   word_t d1 = d[n - 1];

   ASSERT(q != d);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((ci < d1) 
      || ((ci == d1) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((long) d1 < 0);

   a += m;

   for (j = m - n; j >= 0; j--)
   {
      a--;
      
      divapprox21_preinv2(q[j], ci, a[0], dinv);
      
	  /* a -= d*q1 */
      ci -= nn_submul1(a - n + 1, d, n, q[j]);

      /* correct if remainder is too large */
      if (ci || nn_cmp_m(a - n + 1, d, n) >= 0)
      {
         q[j]++;
         ci -= nn_sub_m(a - n + 1, a - n + 1, d, n);
      }
      
      /* fetch next word now that it has been updated */
      ci = a[0];
   }
}

#endif

/* Turn warning back on */
#pragma GCC diagnostic warning "-Wunused-variable"

#ifndef HAVE_ARCH__nn_divapprox_helper

word_t _nn_divapprox_helper(nn_t q, nn_t a, nn_src_t d, len_t s)
{
   word_t ci;
   len_t i;
   
   nn_sub_m(a + 1, a + 1, d, s + 1);
   ci = a[2] + nn_add1(a + 1, a + 1, 1, d[s]);

   for (i = s - 1; i >= 0; i--)
   {
      q[i] = ~WORD(0);
      ci += nn_add1(a, a, 2, d[i]);
   }

   return ci;
}

#endif

/* GCC thinks d1 is unused, so turn warning off */
#pragma GCC diagnostic ignored "-Wunused-variable"

#ifndef HAVE_ARCH_nn_divapprox_classical_preinv_c

word_t nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv2_t dinv, word_t ci)
{
   word_t cy = 0, d1 = d[n - 1];
   len_t s = m - n + 1; 
   
   ASSERT(q != d);
   ASSERT(q != a);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((long) d1 < 0);

   a += m;

   /* Reduce until n - 2 >= s */
   for (s = m - n; s > n - 2; s--)
   {
      a--;
      divapprox21_preinv2(q[s], ci, a[0], dinv);
      
	  /* a -= d*q1 */
      ci -= nn_submul1(a - n + 1, d, n, q[s]);

      /* correct if remainder is too large */
      if (ci || nn_cmp_m(a - n + 1, d, n) >= 0)
      {
         q[s]++;
         ci -= nn_sub_m(a - n + 1, a - n + 1, d, n);
      }
      
      /* fetch next word now that it has been updated */
      cy = ci;
      ci = a[0];
   }
   
   s++;
   d = d + n - s - 1; /* make d length s + 1 */
   
   for ( ; s > 0; s--)
   {
      a--;
      /* rare case where truncation ruins normalisation */
      if (ci > d[s] || (ci == d[s] && nn_cmp_m(a - s + 1, d, s) >= 0))
         return _nn_divapprox_helper(q, a - s, d, s);

      divapprox21_preinv2(q[s - 1], ci, a[0], dinv);
         
      /* a -= d*q1 */
      ci -= nn_submul1(a - s, d, s + 1, q[s - 1]);

	   /* correct if remainder is too large */
      if (ci || nn_cmp_m(a - s, d, s + 1) >= 0)
      {
         q[s - 1]++;
         ci -= nn_sub_m(a - s, a - s, d, s + 1);
      }
      
      /* fetch next word now that it has been updated */
      cy = ci;
      ci = a[0];
      
      d++;
   }  

   return cy;
}

#endif

/* turn warning back on */
#pragma GCC diagnostic warning "-Wunused-variable"

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
