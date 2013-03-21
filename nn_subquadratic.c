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
   word_t ci;

   nn_t t;
   TMP_INIT;

   ASSERT(m >= n);
   ASSERT(n > m2);
   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(n > 1);

   p[m2]       = nn_add(p, a, m2, a + m2, h1);
   p[2*m2 + 1] = nn_add(p + m2 + 1, b, m2, b + m2, h2);
   
   TMP_START;
   t = TMP_ALLOC(2*m2 + 2);
   
   nn_mul_m(t, p + m2 + 1, p, m2 + 1); 
   
   nn_mul_m(p, a, b, m2);
   nn_mul(p + 2*m2, a + m2, h1, b + m2, h2);
   
   ci = -nn_sub(t, t, 2*m2 + 1, p, 2*m2);
   t[2*m2 + 1] = ci - nn_sub(t, t, 2*m2 + 1, p + 2*m2, h1 + h2);
   
   nn_add(p + m2, p + m2, m + h2, t, 2*m2 + 1);
   
   TMP_END;
}

#endif

#ifndef HAVE_ARCH_nn_mul_toom33

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
   
   TMP_START;
   t = TMP_ALLOC(6*m3 + 6);

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

#pragma GCC diagnostic ignored "-Wunused-value"
   ASSERT_ALWAYS(nn_divrem1_preinv(r3, r3, 2*m3 + 2, WORD(3) << norm, ninv) == 0);
#pragma GCC diagnostic warning "-Wunused-value"


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

#endif

#ifndef HAVE_ARCH_nn_mul_toom32

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

   TMP_START;
   t = TMP_ALLOC(4*m3 + 4);

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

#pragma GCC diagnostic ignored "-Wunused-value"
   ASSERT_ALWAYS(nn_shr(r3, r3, 2*m3 + 1, 1) == 0);
#pragma GCC diagnostic warning "-Wunused-value"

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

word_t nn_divapprox_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_t dinv, word_t ci)
{
   len_t s = m - n + 1;
   len_t sh = s/2;
   len_t sl = s - sh;
   nn_t t;
   len_t i;
   
   TMP_INIT;

   ASSERT(n >= s); /* temporary condition */
   ASSERT(DIVAPPROX_CLASSICAL_CUTOFF >= 3);

   /* Base case */
   if (s <= DIVAPPROX_CLASSICAL_CUTOFF)
      return nn_divapprox_classical_preinv_c(q, a, m, d, n, dinv, ci);

   /* Rare case where truncation ruins normalisation */
   if (ci == d[n - 1] && nn_equal_m(a + m - s + 1, d + n - s, s - 1))
      return nn_divapprox_classical_preinv_c(q, a, m, d, n, dinv, ci); /* Todo: do this efficiently */

   TMP_START;
   t = TMP_ALLOC(sl + 2);

   ci = nn_divapprox_divconquer_preinv_c(q + sl, a + sl, 
                                             n + sh - 1, d, n, dinv, ci);
   
   nn_mulmid_classical(t + sl, t, d + n - s - 2, s, q + sl, sh); /* Todo : switch to fast mulmid */
   ci -= nn_sub_m(a + m - s - 1, a + m - s - 1, t, sl + 2);
   
   while ((long) ci < 0)
   {
      nn_sub1(q + sl, q + sl, sh, 1); /* ensure quotient is not too big */

      /*
         correct remainder, noting that "digits" of quotient are not base B
         but in a base that varies with truncation, thus the correction
         needs a fixup
      */
      ci += nn_add_m(a + m - s - 1, a + m - s - 1, d + n - sl - 2, sl + 2); 

      for (i = 0; q[sl + i] == ~WORD(0) && i < sh - 1; i++)
         ci += nn_add1(a + m - s - 1, a + m - s - 1, sl + 2, d[n - sl - 3 - i]);
   }
   
   ASSERT(ci == 0); /* Todo: write out all binary 1's for quotient here */

   ci = nn_divapprox_divconquer_preinv_c(q, a, n + sl - 1, d, n, dinv, a[m - sh]);

   TMP_END;

   return ci;
}

#endif
