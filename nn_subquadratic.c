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
#include "nn_subquadratic_arch.h"

#ifndef HAVE_ARCH_nn_mul_kara

void nn_mul_kara(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t m2 = (m + 1)/2;
   len_t h1 = m - m2;
   len_t h2 = n - m2;
   
   nn_t t;
   TMP_INIT;

   ASSERT(m >= n);
   ASSERT(n > m2);
   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(n >= 2);

   p[m2]       = nn_add(p, a, m2, a + m2, h1);
   p[2*m2 + 1] = nn_add(p + m2 + 1, b, m2, b + m2, h2);
   
   TMP_START;
   t = (nn_t) TMP_ALLOC(2*m2 + 2);
   
   nn_mul_m(t, p + m2 + 1, p, m2 + 1); 
   
   nn_mul_m(p, a, b, m2);
   nn_mul(p + 2*m2, a + m2, h1, b + m2, h2);
   
   nn_sub(t, t, 2*m2 + 1, p, 2*m2);
   nn_sub(t, t, 2*m2 + 1, p + 2*m2, h1 + h2);
   
   /* add a_hi*b_lo + a_lo*b_hi, cannot exceed m + 1 words */
   nn_add(p + m2, p + m2, m + h2, t, m + 1);
   
   TMP_END;
}

#endif

#ifndef HAVE_ARCH_nn_mul_toom33

#pragma GCC diagnostic ignored "-Wunused-value"

void nn_mul_toom33(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t m3 = (m + 2)/3;
   len_t h1 = m - 2*m3;
   len_t h2 = n - 2*m3;
   len_t nn;
   word_t ninv, ci;
   bits_t norm;
   nn_t t;
   TMP_INIT;

   ASSERT(n > 2);
   ASSERT(m >= n);
   ASSERT(n > 2*m3);
   ASSERT(p != a);
   ASSERT(p != b);
   
   TMP_START;
   t = (nn_t) TMP_ALLOC(6*m3 + 6);

#define r1 p
#define r2 t
#define r3 (t + 2*m3 + 2)
#define r4 (t + 4*m3 + 4)  
#define r5 (p + 4*m3) 
#define s1 (p + m3 + 1)

   r1[m3]  = nn_add_m(r1, a, a + m3, m3); /* Evaluate at 1 */
   r1[m3] += nn_add(r1, r1, m3, a + 2*m3, h1);
   s1[m3]  = nn_add_m(s1, b, b + m3, m3);
   s1[m3] += nn_add(s1, s1, m3, b + 2*m3, h2);
   nn_mul_m(r2, r1, s1, m3 + 1);

   ASSERT(r1[m3] < 3);
   ASSERT(s1[m3] < 3);

   ci = nn_shl(r1, a + 2*m3, h1, 1); /* Evaluate at 2 */
   r1[m3]  = nn_add(r1, a + m3, m3, r1, h1);
   r1[m3] += nn_add1(r1 + h1, r1 + h1, m3 - h1, ci);
   nn_shl(r1, r1, m3 + 1, 1);
   r1[m3] += nn_add_m(r1, r1, a, m3);
   ci = nn_shl(s1, b + 2*m3, h2, 1);
   s1[m3]  = nn_add(s1, b + m3, m3, s1, h2);
   s1[m3] += nn_add1(s1 + h2, s1 + h2, m3 - h2, ci);
   nn_shl(s1, s1, m3 + 1, 1);
   s1[m3] += nn_add_m(s1, s1, b, m3);
   nn_mul_m(r3, r1, s1, m3 + 1);

   ASSERT(r1[m3] < 7);
   ASSERT(s1[m3] < 7);

   ci = nn_shl(r1, a + 2*m3, h1, 2); /* Evaluate at 4 */
   r1[m3]  = nn_add(r1, a + m3, m3, r1, h1);
   r1[m3] += nn_add1(r1 + h1, r1 + h1, m3 - h1, ci);
   nn_shl(r1, r1, m3 + 1, 2);
   r1[m3] += nn_add_m(r1, r1, a, m3);
   ci = nn_shl(s1, b + 2*m3, h2, 2);
   s1[m3]  = nn_add(s1, b + m3, m3, s1, h2);
   s1[m3] += nn_add1(s1 + h2, s1 + h2, m3 - h2, ci);
   nn_shl(s1, s1, m3 + 1, 2);
   s1[m3] += nn_add_m(s1, s1, b, m3);
   nn_mul_m(r4, r1, s1, m3 + 1);

   ASSERT(r1[m3] < 21);
   ASSERT(s1[m3] < 21);

   nn_mul_m(r1, a, b, m3); /* Evaluate at 0 */
   nn_mul(r5, a + 2*m3, h1, b + 2*m3, h2); /* Evaluate at oo */

   nn_zero(p + 2*m3, 2*m3);

   r3[2*m3 + 1] = -nn_sub(r3, r3, 2*m3 + 1, r1, 2*m3); /* Interpolate */
   r4[2*m3 + 1] = -nn_sub(r4, r4, 2*m3 + 1, r1, 2*m3);
   r2[2*m3 + 1] = -nn_sub(r2, r2, 2*m3 + 1, r1, 2*m3);
   nn_submul1(r3, r2, 2*m3 + 1, 2);
   nn_submul1(r4, r2, 2*m3 + 1, 4);
   ci = nn_submul1(r4, r5, h1 + h2, 112);
   nn_sub1(r4 + h1 + h2, r4 + h1 + h2, 2*m3 - h1 - h2 + 1, ci); 
   nn_submul1(r4, r3, 2*m3 + 1, 10);
   nn_neg(r4, r4, 2*m3 + 1);
   nn_shr(r4, r4, 2*m3 + 1, 3);
   nn_submul1(r3, r4, 2*m3 + 1, 2);
   ci = nn_submul1(r3, r5, h1 + h2, 14);
   nn_sub1(r3 + h1 + h2, r3 + h1 + h2, 2*m3 - h1 - h2 + 1, ci);

   norm = high_zero_bits(WORD(3));
   ninv = precompute_inverse1(WORD(3) << norm);
   r3[2*m3 + 1] = nn_shl(r3, r3, 2*m3 + 1, norm);

   ASSERT_ALWAYS(nn_divrem1_preinv(r3, r3, 2*m3 + 2, WORD(3) << norm, ninv) == 0);

   nn_shr(r3, r3, 2*m3 + 1, 1);
   nn_sub_m(r2, r2, r3, 2*m3 + 1);
   nn_sub_m(r2, r2, r4, 2*m3 + 1);
   nn_sub(r2, r2, 2*m3 + 1, r5, h1 + h2);

   nn = nn_normalise(r3, 2*m3 + 1); /* Normalise */
   
   nn_add(p + m3, p + m3, 3*m3 + h1 + h2, r2, 2*m3 + 1); /* Recombine */
   nn_add(p + 2*m3, p + 2*m3, 2*m3 + h1 + h2, r4, 2*m3 + 1); 
   nn_add(p + 3*m3, p + 3*m3, m3 + h1 + h2, r3, nn); 

   TMP_END;

#undef r1
#undef r2
#undef r3
#undef r4
#undef r5
#undef s1
}

#pragma GCC diagnostic warning "-Wunused-value"

#endif

#ifndef HAVE_ARCH_nn_mul_toom32

#pragma GCC diagnostic ignored "-Wunused-value"

void nn_mul_toom32(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t m3 = (m + 2)/3;
   len_t h1 = m - 2*m3;
   len_t h2 = n - m3;
   len_t nn;
   nn_t t;
   word_t ci;
   TMP_INIT;

   ASSERT(n > 4);
   ASSERT(n <= 2*m3);
   ASSERT(n > m3);
   ASSERT(p != a);
   ASSERT(p != b);

   TMP_START;
   t = (nn_t) TMP_ALLOC(4*m3 + 4);

#define r1 p
#define r2 t
#define r3 (t + 2*m3 + 2)
#define r4 (p + 3*m3)   
#define s1 (p + m3 + 1)

   r1[m3]  = nn_add(r1, a, m3, a + 2*m3, h1); /* Evaluate at 1 */
   r1[m3] += nn_add_m(r1, r1, a + m3, m3);
   s1[m3]  = nn_add(s1, b, m3, b + m3, h2);
   nn_mul_m(r2, r1, s1, m3 + 1);

   ASSERT(r1[m3] < 3);
   ASSERT(s1[m3] < 2);

   ci = nn_shl(r1, a + 2*m3, h1, 1); /* Evaluate at 2 */
   r1[m3]  = nn_add(r1, a + m3, m3, r1, h1);
   r1[m3] += nn_add1(r1 + h1, r1 + h1, m3 - h1, ci);
   nn_shl(r1, r1, m3 + 1, 1);
   r1[m3] += nn_add_m(r1, r1, a, m3);
   ci = nn_shl(s1, b + m3, h2, 1);
   s1[m3]  = nn_add(s1, b, m3, s1, h2);
   s1[m3] += nn_add1(s1 + h2, s1 + h2, m3 - h2, ci);
   nn_mul_m(r3, r1, s1, m3 + 1);

   ASSERT(r1[m3] < 7);
   ASSERT(s1[m3] < 3);

   nn_mul_m(r1, a, b, m3); /* Evaluate at 0 */
   if (h1 >= h2) nn_mul(r4, a + 2*m3, h1, b + m3, h2); /* Evaluate at oo */
   else nn_mul(r4, b + m3, h2, a + 2*m3, h1);
   nn_zero(p + 2*m3, m3);

   nn_sub_m(r2, r2, r1, 2*m3 + 1); /* Interpolate */
   nn_sub_m(r3, r3, r1, 2*m3 + 1);
   nn_submul1(r3, r2, 2*m3 + 1, 2); 
   ci = nn_submul1(r3, r4, h1 + h2, 6);
   nn_sub1(r3 + h1 + h2, r3 + h1 + h2, 2*m3 - h1 - h2 + 1, ci);

   ASSERT_ALWAYS(nn_shr(r3, r3, 2*m3 + 1, 1) == 0);

   nn_sub(r2, r2, 2*m3 + 1, r4, h1 + h2);
   nn_sub_m(r2, r2, r3, 2*m3 + 1);
   
   nn = nn_normalise(r3, 2*m3 + 1); /* Normalise */
   
   nn_add(p + m3, p + m3, 2*m3 + h1 + h2, r2, 2*m3 + 1); /* Recombine */
   nn_add(p + 2*m3, p + 2*m3, m3 + h1 + h2, r3, nn); 

   TMP_END;

#undef r1
#undef r2
#undef r3
#undef r4
#undef s1
}

#pragma GCC diagnostic warning "-Wunused-value"

#endif

#ifndef HAVE_ARCH_nn_mulmid_kara

void nn_mulmid_kara(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
  len_t i, n2 = n/2, odd = (n & 1), extra = m - 4*n2 + 1 - odd;
  nn_t s, p0, p1, p2, f, ex = NULL;
  int neg = 0;
  nn_t ptr;
  dword_t t;
  word_t ci, tmp[2], tmp2[2];

  TMP_INIT;

  ASSERT(p != a);
  ASSERT(p != b);
  ASSERT(MULMID_CLASSICAL_CUTOFF >= 3);
  ASSERT(n >= 2);
  ASSERT(m >= 2*n - 1);

  if (n <= MULMID_CLASSICAL_CUTOFF)
  {
     nn_mulmid_classical(ov, p, a, m, b, n);
     return;
  }

  if (nn_cmp_m(b, b + n2, n2) < 0)
     neg = 1;

  TMP_START;

  s = (nn_t) TMP_ALLOC(2*n2 - 1);
  p0 = (nn_t) TMP_ALLOC(n2 + 2);
  p1 = (nn_t) TMP_ALLOC(n2 + 2);
  p2 = (nn_t) TMP_ALLOC(n2 + 2);
  f = (nn_t) TMP_ALLOC(n2 + 2);

  if (extra) ptr = tmp2, ex = (nn_t) TMP_ALLOC(extra);
  else ptr = ov;

  if (odd) a++;

  /* p0 = (a0 + a1)*b1 */
  _nn_mulmid_add_lfix_m(s, f + n2, f, a, a + n2, b + n2, n2);
  nn_mulmid_kara(p0 + n2, p0, s, 2*n2 - 1, b + n2, n2);
  nn_add_m(p0, p0, f, n2 + 2);

  /* p1 = a1*(b0 - b1) */
  if (neg) _nn_mulmid_sub_rfix_m(s, f + n2, f, a + n2, b + n2, b, n2);
  else _nn_mulmid_sub_rfix_m(s, f + n2, f, a + n2, b, b + n2, n2);
  nn_mulmid_kara(p1 + n2, p1, a + n2, 2*n2 - 1, s, n2);
  nn_sub_m(p1, p1, f, n2 + 2);

  /* p2 = (a1 + a2)*b0 */
  _nn_mulmid_add_lfix_m(s, f + n2, f, a + n2, a + 2*n2, b, n2);
  nn_mulmid_kara(p2 + n2, p2, s, 2*n2 - 1, b, n2);
  nn_add_m(p2, p2, f, n2 + 2);

  /* r0 = p0 + p1, r1 = p2 - p1 */
  if (neg) 
  {
     nn_sub_m(p, p0, p1, n2 + 2);
     tmp[0] = p[n2], tmp[1] = p[n2 + 1];
     ci = nn_add_m(p + n2, p2, p1, n2);
     nn_add_mc(ptr, p2 + n2, p1 + n2, 2, ci);
  } else 
  {
     nn_add_m(p, p0, p1, n2 + 2);
     tmp[0] = p[n2], tmp[1] = p[n2 + 1];
     ci = nn_sub_m(p + n2, p2, p1, n2);
     nn_sub_mc(ptr, p2 + n2, p1 + n2, 2, ci);
  }
  
  ci = nn_add(p + n2, p + n2, n2, tmp, 2);  
  nn_add1(ptr, ptr, 2, ci);

  if (extra)
  {
     a += 4*n2 - 1;

     t = (dword_t) nn_mul1(ex, a, extra, b[0]);
     for (i = 1, a--; i < 2*n2; i++, a--)
        t += (dword_t) nn_addmul1(ex, a, extra, b[i]);

     if (extra == 1)
        t += (dword_t) nn_add1(p + 2*n2, ex, 1, ptr[0]) + (dword_t) ptr[1];
     else
        t += (dword_t) nn_add(p + 2*n2, ex, extra, ptr, 2);
     
     if (odd)
     {
        a -= 2*n2;
        t += nn_addmul1(p, a, 2*n2 + extra, b[i]);
     }

     ov[0] = (word_t) t;
     ov[1] = (word_t) (t >> WORD_BITS);
  }

  TMP_END;
}

#endif

#ifndef HAVE_ARCH_nn_mullow_kara_m

void nn_mullow_kara_m(nn_t ov, nn_t p, nn_src_t a, nn_src_t b, len_t n)
{
   len_t nl = n/2, nh = n - nl;
   nn_t p1, p2;
   word_t ci;

   TMP_INIT;

   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(MULLOW_CLASSICAL_CUTOFF >= 1);
   ASSERT(n >= 2);

   nn_mul(p, b, nh, a, nl);
   
   TMP_START;

   p1 = (nn_t) TMP_ALLOC(nh);
   p2 = (nn_t) TMP_ALLOC(nl + 2);
   
   nn_mullow_m(ov, p1, a + nl, b, nh);
   nn_mullow_m(p2 + nl, p2, a, b + nh, nl);

   ci = nn_add_m(p + nl, p + nl, p1, nh);
   nn_add1(ov, ov, 2, ci);

   ci = nn_add_m(p + nh, p + nh, p2, nl);
   nn_add_mc(ov, ov, p2 + nl, 2, ci);
   
   TMP_END;
}

#endif

#ifndef HAVE_ARCH_nn_mullow_kara

void nn_mullow_kara(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t nl = n/2, nh = n - nl;
   len_t mh = nl, ml = m - mh;
   nn_t p1, p2;
   word_t ci;

   TMP_INIT;

   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(MULLOW_CLASSICAL_CUTOFF >= 1);
   ASSERT(m >= n);
   ASSERT(n >= 0);

   if (n <= MULLOW_CLASSICAL_CUTOFF)
   {
      nn_mullow_classical(ov, p, a, m, b, n);
      return;
   }

   nn_mul(p, a, ml, b, nl);
   
   TMP_START;

   p1 = (nn_t) TMP_ALLOC(mh);
   p2 = (nn_t) TMP_ALLOC(ml + 2);

   nn_mullow_kara(ov, p1, a + ml, mh, b, mh);
   nn_mullow_kara(p2 + ml, p2, a, ml, b + nl, nh);

   ci = nn_add_m(p + ml, p + ml, p1, mh);
   nn_add1(ov, ov, 2, ci);

   ci = nn_add_m(p + nl, p + nl, p2, ml);
   nn_add_mc(ov, ov, p2 + ml, 2, ci);
   
   TMP_END;
}

#endif

#ifndef HAVE_ARCH_nn_divapprox_divconquer_preinv_c

word_t nn_divapprox_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                            len_t n, preinv2_t dinv, word_t ci)
{
   len_t s = m - n + 1;
   len_t sh, sl;
   nn_t t;
   len_t i;
   
   TMP_INIT;

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(n >= 2);
   ASSERT((ci < d[n - 1]) 
      || ((ci == d[n - 1]) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((sword_t) d[n - 1] < 0);

   /* special case, s <= 3 */
   if (s <= 3 || n <= 5) return nn_divapprox_classical_preinv_c(q, a, m, d, n, dinv, ci);

   /* Reduce until n - 2 >= s */
   while (n - 2 < s)
   {
      sh = BSDNT_MIN(n, s - n + 2);
      nn_divrem_divconquer_preinv_c(q + s - sh, a + m - n - sh + 1, 
                                                   n + sh - 1, d, n, dinv, ci);
      s -= sh; m -= sh; 
      ci = a[m];
   }

   /* split into two parts */
   sh = s/2; sl = s - sh;

   /* Rare case where truncation ruins normalisation */
   if (ci > d[n - 1] || (ci == d[n - 1] 
     && nn_cmp_m(a + m - s + 1, d + n - s, s - 1) >= 0))
      return _nn_divapprox_helper(q, a + m - s - 1, d + n - s - 1, s);

   ci = nn_divapprox_preinv_c(q + sl, a + sl, n + sh - 1, d, n, dinv, ci);

   TMP_START;
   t = (nn_t) TMP_ALLOC(sl + 2);

   nn_mulmid_kara(t + sl, t, d + n - s - 1, s - 1, q + sl, sh);
   ci -= nn_sub_m(a + m - s - 1, a + m - s - 1, t, sl + 2);

   TMP_END;

   if ((sword_t) ci < 0)
   {
      
      nn_sub1(q + sl, q + sl, sh, 1); /* ensure quotient is not too big */

      /*
         correct remainder, noting that "digits" of quotient aren't base B
         but in base varying with truncation, thus correction needs fixup
      */
      ci += nn_add_m(a + m - s - 1, a + m - s - 1, d + n - sl - 2, sl + 2); 

      for (i = 0; i < sh - 1 && q[sl + i] == ~WORD(0); i++)
         ci += nn_add1(a + m - s - 1, a + m - s - 1, sl + 2, d[n - sl - 3 - i]);
   }
   
   if (ci != 0 || nn_cmp_m(a + sl, d, n) >= 0) /* special case: unable to canonicalise */
      return _nn_divapprox_helper(q, a + m - s - 1, d + n - sl - 1, sl);

   ci = nn_divapprox_preinv_c(q, a, n + sl - 1, d, n, dinv, a[m - sh]);

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem_divconquer_preinv_c

void nn_divrem_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                            len_t n, preinv2_t dinv, word_t ci)
{
   len_t sh, s = m - n + 1;
   nn_t t;

   TMP_INIT;

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(m >= n);
   ASSERT(n >= 2);
   ASSERT((ci < d[n - 1]) 
      || ((ci == d[n - 1]) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((sword_t) d[n - 1] < 0);

   /* Base case */
   if (n <= 3 || s <= 1)
   {
      nn_divrem_classical_preinv_c(q, a, m, d, n, dinv, ci);
      return;
   }

   /* Reduce until n - 2 >= s */
   while (n - 2 < s)
   {
      sh = BSDNT_MIN(n, s - n + 2);
      nn_divrem_preinv_c(q + s - sh, a + m - n - sh + 1, 
                                                   n + sh - 1, d, n, dinv, ci);
      s -= sh; m -= sh; 
      ci = a[m];
   }

   ci = nn_divapprox_preinv_c(q, a, m, d, n, dinv, ci);

   TMP_START;
   t = (nn_t) TMP_ALLOC(n);

   if (s > 0)
   {
      nn_mullow_kara(t + n - 2, t, d, n - 2, q, s);
      ci -= nn_sub_m(a, a, t, n);
   } 
   
   TMP_END;

   while ((sword_t) ci < 0)
   {
      nn_sub1(q, q, s, 1); /* ensure quotient is not too big */

      ci += nn_add_m(a, a, d, n); 
   } 

   ASSERT(ci == 0);
}

#endif

#ifndef HAVE_ARCH_nn_div_divconquer_preinv_c

void nn_div_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                            len_t n, preinv2_t dinv, word_t ci)
{
   len_t s = m - n + 1;
   nn_t t, t2;

   TMP_INIT;

   ASSERT(q != a);
   ASSERT(q != d);
   ASSERT(m >= n);
   ASSERT(n >= 2);
   ASSERT((ci < d[n - 1]) 
      || ((ci == d[n - 1]) && (nn_cmp_m(a + m - n + 1, d, n - 1) < 0)));
   ASSERT((sword_t) d[n - 1] < 0);

   TMP_START;
   
   t = (nn_t) TMP_ALLOC(s + 1);
   t2 = (nn_t) TMP_ALLOC(m + 1);

   nn_copy(t2 + 1, a, m);
   t2[0] = 0;

   /* compute one extra limb of the quotient */
   nn_divapprox_preinv_c(t, t2, m + 1, d, n, dinv, ci);

   if (-t[0] <= 1)
   {
      if (s >= n) nn_mul(t2, t + 1, s, d, n);
      else nn_mul(t2, d, n, t + 1, s);

      if (m > n)
      {
         nn_sub_m(a, a, t2, n + 1);
         ci = a[n];
      } else
         ci -= t2[n] + nn_sub_m(a, a, t2, n);         

      /* quotient may be one too small or too large */
      if ((sword_t) ci < 0)
         nn_sub1(q, t + 1, s, 1);
      else if (ci || nn_cmp_m(a, d, n) >= 0)
         nn_add1(q, t + 1, s, 1);
      else
         nn_copy(q, t + 1, s);
   } else
      nn_copy(q, t + 1, s);

   TMP_END;
}

#endif
