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

#include <math.h>
#include <string.h> /* strspn */
#include "nn.h"
#include "nn_quadratic_arch.h"
#include "zz0.h"

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
  ASSERT(p != a);
  ASSERT(p != b);

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

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(a != d);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((ci < d1) 
      || ((ci == d1) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((sword_t) d1 < 0);

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
   
   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(a != d);
   ASSERT(m >= n);
   ASSERT(n > 1);
   ASSERT((ci < d1) 
      || ((ci == d1) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((sword_t) d1 < 0);

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
   
   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(a != d);
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

#ifndef HAVE_ARCH___get_bits_lehmer

#define __get_bits_lehmer(rr1, rr2, a, m, b, n) \
   do { \
      word_t __ah = (a)[(m) - 1]; \
      bits_t __bits = high_zero_bits(__ah); \
      if (m >= n + 1) { \
         if (__bits == 0) (rr1) = 0, (rr2) = (__ah >> 1); \
         else if (__bits == 1) (rr1) = 0, (rr2) = __ah; \
         else { \
            __bits--; \
           (rr2) = (__ah << __bits) + ((a)[(m) - 2] >> (WORD_BITS - __bits)); \
           if (m == n + 1) (rr1) = ((b)[(n) - 1] >> (WORD_BITS - __bits)); \
           else (rr1) = 0; \
         } \
      } else { \
         word_t __bh = b[n - 1]; \
         __bits = BSDNT_MIN(__bits, high_zero_bits(__bh)); \
         if (__bits == 0) (rr1) = (__bh >> 1), (rr2) = (__ah >> 1); \
         else if (__bits == 1) (rr1) = __bh, (rr2) = __ah; \
         else { \
            __bits--; \
            if (n == 1) (rr1) = (__bh << __bits), (rr2) = (__ah << __bits); \
            else { \
               (rr2) = (__ah << __bits) + ((a)[(m) - 2] >> (WORD_BITS - __bits)); \
               (rr1) = (__bh << __bits) + ((b)[(m) - 2] >> (WORD_BITS - __bits)); \
            } \
         } \
      } \
   } while (0)

#endif

#ifndef HAVE_ARCH_nn_xgcd_lehmer

len_t nn_xgcd_lehmer(nn_t g, nn_t v, nn_t a, len_t m, nn_t b, len_t n)
{
   nn_t q, r, c1, c2, a2;
   long aa2, aa1, bb2, bb1, rr1 = 0, rr2 = 0, qq, t1, t2, t3, i;
   len_t s1, s2, s1u, s2u, s, m_orig = m;
   word_t ci, aci, rci;
   TMP_INIT;

   ASSERT(m >= n);
   ASSERT(n > 0);
   ASSERT(a != b);
   ASSERT(a[m - 1] != 0);
   ASSERT(b[n - 1] != 0);
   
   TMP_START;
   
   q = (nn_t) TMP_ALLOC(m + 1);
   r = (nn_t) TMP_ALLOC(m + 1);
   c1 = (nn_t) TMP_ALLOC(m + 1);
   c2 = (nn_t) TMP_ALLOC(m + 1);
   a2 = (nn_t) TMP_ALLOC(m);

   nn_copy(a2, a, m);

   nn_zero(c2, m);
   nn_zero(c1, m);
   c2[0] = 1;
   s2 = 1;
   s1 = 0;
   
   while (n > 0)
   {
      __get_bits_lehmer(rr1, rr2, a, m, b, n);

      aa2 = 0; aa1 = 1;
      bb2 = 1; bb1 = 0;

      for (i = 0; rr1 != 0; i++)
      {
         qq = rr2 / rr1;

         t1 = rr2 - qq*rr1; 
         t2 = aa2 - qq*aa1; 
         t3 = bb2 - qq*bb1; 

         if (i & 1)
         {
            if (t1 < -t3 || rr1 - t1 < t2 - aa1) break;
         } else 
         {
            if (t1 < -t2 || rr1 - t1 < t3 - bb1) break;
         }

         rr2 = rr1; rr1 = t1;
         aa2 = aa1; aa1 = t2;
         bb2 = bb1; bb1 = t3;
      }

      if (i == 0)
      {
         nn_divrem(q, a, m, b, n);
         NN_SWAP(a, b);
         s = m - n + 1; m = n; 
         n = nn_normalise(b, n);
         
         s = zz0_mul(r, c2, s2, q, s);
         s1 = zz0_sub(c1, c1, s1, r, s);

         NN_SWAP(c1, c2); 
         BSDNT_SWAP(s1, s2);
      } else
      {
         s1u = BSDNT_ABS(s1);
         s2u = BSDNT_ABS(s2);
            
         if (bb2 <= 0)
         {
            rci = nn_mul1(r, a, m, -bb2);
            ci = nn_submul1(r, b, n, aa2);
            rci -= nn_sub1(r + n, r + n, m - n, ci);

            q[s2u] = nn_mul1(q, c2, s2u, aa2);
            ci = nn_addmul1(q, c1, s1u, -bb2);
            q[s2u] += nn_add1(q + s1u, q + s1u, s2u - s1u, ci);   
         } else
         {
            rci = nn_mul1(r, a, m, bb2);
            ci = nn_submul1(r, b, n, -aa2);
            rci -= nn_sub1(r + n, r + n, m - n, ci);

            q[s2u] = nn_mul1(q, c2, s2u, -aa2);
            ci = nn_addmul1(q, c1, s1u, bb2);
            q[s2u] += nn_add1(q + s1u, q + s1u, s2u - s1u, ci);
            s1 = -s1;
         }
         
         if (aa1 <= 0)
         {
            aci = nn_mul1(a, a, m, bb1);
            ci = nn_submul1(a, b, n, -aa1);
            aci -= nn_sub1(a + n, a + n, m - n, ci);

            c2[s2u] = nn_mul1(c2, c2, s2u, -aa1);
            ci = nn_addmul1(c2, c1, s1u, bb1);
            c2[s2u] += nn_add1(c2 + s1u, c2 + s1u, s2u - s1u, ci);
            s2 = -s2;
         } else
         {
            aci = nn_mul1(a, a, m, -bb1);
            ci = nn_submul1(a, b, n, aa1);
            aci -= nn_sub1(a + n, a + n, m - n, ci);

            c2[s2u] = nn_mul1(c2, c2, s2u, aa1);
            ci = nn_addmul1(c2, c1, s1u, -bb1);
            c2[s2u] += nn_add1(c2 + s1u, c2 + s1u, s2u - s1u, ci);
         }

         if (((sword_t) rci) < 0) { nn_neg(r, r, m); s1 = -s1; }
         if (((sword_t) aci) < 0) { nn_neg(a, a, m); s1 = -s1; }

         n = nn_normalise(r, m);
         m = nn_normalise(a, m);
         
         s1u = ++s2u;
         s1u = nn_normalise(q, s1u);
         s1 = s1 < 0 || s2 > 0 ? -s1u : s1u;
         s2u = nn_normalise(c2, s2u);
         s2 = s2 < 0 || s1 > 0 ? -s2u : s2u;
         
         nn_copy(c1, q, s1u);

         if (nn_cmp(a, m, r, n) < 0)
         {
            nn_copy(b, a, m);
            nn_copy(a, r, n);
            BSDNT_SWAP(m, n);    
         } else {
            nn_copy(b, r, n);
            BSDNT_SWAP(s1, s2);
            NN_SWAP(c1, c2);
         }
      }
   }

   if (s1 > 0) { /* adjust cofactor to be negative */
      ci = nn_sub_m(c1, a2, c1, s1);
      nn_sub1(c1 + s1, a2 + s1, m_orig - s1, ci);
      s1 = -m_orig;
   }

   s1 = -s1;
   if (s1 > 0) /* write out cofactor and zero top limbs */
      nn_copy(v, c1, s1);
   if (m_orig > s1) 
      nn_zero(v + s1, m_orig - s1);

   nn_copy(g, a, m); /* write out gcd */
   
   TMP_END;

   return m;
}

#endif

#ifndef HAVE_ARCH_nn_gcd_lehmer

len_t nn_gcd_lehmer(nn_t g, nn_t a, len_t m, nn_t b, len_t n)
{
   nn_t q, r, s;
   sword_t aa2, aa1, bb2, bb1, rr1 = 0, rr2 = 0, qq, t1, t2, t3, i;
   word_t ci, c1, c2;
   TMP_INIT;

   ASSERT(m >= n);
   ASSERT(n > 0);
   ASSERT(a != b);
   ASSERT(a[m - 1] != 0);
   ASSERT(b[n - 1] != 0);
   
   TMP_START;
   
   q = (nn_t) TMP_ALLOC(m);
   r = (nn_t) TMP_ALLOC(m + 1);
   
   if (m != n)
   {
      s = (nn_t) TMP_ALLOC(m);
      nn_copy(s, b, n);
      b = s;
   }
   
   while (n > 0)
   {
      __get_bits_lehmer(rr1, rr2, a, m, b, n);

      aa2 = 0; aa1 = 1;
      bb2 = 1; bb1 = 0;

      for (i = 0; rr1 != 0; i++)
      {
         qq = rr2 / rr1;

         t1 = rr2 - qq*rr1; 
         t2 = aa2 - qq*aa1; 
         t3 = bb2 - qq*bb1; 

         if (i & 1)
         {
            if (t1 < -t3 || rr1 - t1 < t2 - aa1) break;
         } else 
         {
            if (t1 < -t2 || rr1 - t1 < t3 - bb1) break;
         }

         rr2 = rr1; rr1 = t1;
         aa2 = aa1; aa1 = t2;
         bb2 = bb1; bb1 = t3;
      }

      if (i == 0)
      {
         nn_divrem(q, a, m, b, n);
         NN_SWAP(a, b);
         m = n; 
         n = nn_normalise(b, n);
      } else
      {
         if (bb2 <= 0)
         {
            c1 = nn_mul1(r, a, m, -bb2);
            ci = nn_submul1(r, b, n, aa2);
            c1 -= nn_sub1(r + n, r + n, m - n, ci);
         } else
         {
            c1 = nn_mul1(r, a, m, bb2);
            ci = nn_submul1(r, b, n, -aa2);
            c1 -= nn_sub1(r + n, r + n, m - n, ci);
         }
         
         if (aa1 <= 0)
         {
            c2 = nn_mul1(a, a, m, bb1);
            ci = nn_submul1(a, b, n, -aa1);
            c2 -= nn_sub1(a + n, a + n, m - n, ci);
         } else
         {
            c2 = nn_mul1(a, a, m, -bb1);
            ci = nn_submul1(a, b, n, aa1);
            c2 -= nn_sub1(a + n, a + n, m - n, ci);
         }
      
         n = m; 
         if ((sword_t) c1 < 0) nn_neg(r, r, n);
         if ((sword_t) c2 < 0) nn_neg(a, a, n);

         n = nn_normalise(r, n);
         m = nn_normalise(a, m);
         
         if (nn_cmp(a, m, r, n) < 0)
         {
            nn_copy(b, a, m);
            nn_copy(a, r, n);
            BSDNT_SWAP(m, n);
         } else
            nn_copy(b, r, n);
      }
   }

   nn_copy(g, a, m); /* write out gcd */
      
   TMP_END;

   return m;
}

#endif

#ifndef HAVE_ARCH_nn_get_str

char * nn_get_str(nn_src_t a, len_t m)
{
   /* 9.63... is log_10(2^32) */
   size_t i = 0, j;
   size_t digits = (long) ceil(m * 9.632959861247398 * (WORD_BITS/32)) + (m == 0);
   char * str = (char *) malloc(digits + 1);
   word_t ci, d = 10UL << (WORD_BITS - 4);
   nn_t q1, q2, t;
   TMP_INIT;

   if (m == 0)
      str[0] = '0';
   else 
   {
      TMP_START;
      q1 = (nn_t) TMP_ALLOC(m);
      q2 = (nn_t) TMP_ALLOC(m);
      nn_copy(q1, a, m);

      /* compute digits in reverse order */
      for (i = digits; m > 0; i--) 
      {
         ci = nn_shl(q1, q1, m, WORD_BITS - 4);
         str[i - 1] = 48 + (char) (nn_divrem1_simple_c(q2, q1, m, d, ci) >> (WORD_BITS - 4));
         t = q1; q1 = q2; q2 = t;
         m = nn_normalise(q1, m);
      }

      TMP_END;

      /* didn't get the right number of digits, shift */
      if (i) 
      {
         for (j = i; j < digits; j++)
            str[j - i] = str[j];
      }
   }

   str[digits - i] = '\0';

   return str;
}

#endif

#ifndef HAVE_ARCH_set_str

size_t nn_set_str(nn_t a, len_t * len, const char * str)
{
   len_t m = 1;
   size_t i, digits = strspn(str, "0123456789");
   word_t ci;

   if (digits == 1 && str[0] == '0')
   {
      *len = 0;
      return 1;
   }

   a[0] = (word_t) str[0] - 48;
   for (i = 1; i < digits; i++) 
   {
      ci = nn_mul1(a, a, m, 10);
      ci += nn_add1(a, a, m, (word_t) str[i] - 48);
      if (ci) a[m++] = ci;
   }

   *len = m;
   return digits;
}

#endif


#ifndef HAVE_ARCH_get_str_raw

size_t nn_get_str_raw(unsigned char *str, int base, nn_t a, len_t len)
{
   size_t i = 0;

   len = nn_normalise(a, len);

   if (len == 0)
      return 0;

   while (len > 0)
   {
      str[i++] = nn_divrem1_simple(a, a, len, base);
      len = nn_normalise(a, len);
   }

   /* Reverse str to big-endian order */
   unsigned char *x = str;
   unsigned char *y = str + i - 1;
   while (x < y)
   {
      char c = *x;
      *(x++) = *y;
      *(y--) = c;
   }

  return i;
}

#endif

#ifndef HAVE_ARCH_set_str_raw

len_t nn_set_str_raw(nn_t a, const unsigned char * str, size_t size, int base)
{
   size_t i;
   len_t m = 1;
   word_t ci;

   if (size == 0)
      return 0;

   a[0] = (word_t) str[0];
   for (i = 1; i < size; i++)
   {
      ci = nn_mul1(a, a, m, base);
      ci += nn_add1(a, a, m, (word_t)str[i]);
      if (ci)
         a[m++] = ci;
   }

  return m;
}

#endif

#ifndef HAVE_ARCH_nn_print

void nn_print(nn_src_t a, len_t m)
{
   char * str = nn_get_str(a, m);

   printf("%s", str);

   free(str);
}

#endif
