/* 
  Copyright (C) 2010, 2013 William Hart
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

#include <stdio.h>
#include "nn.h"
#include "nn_linear_arch.h"

/**********************************************************************
 
    Random generation

**********************************************************************/

void nn_random(nn_t a, rand_t state, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = (word_t) state.word(state.ctx);
}

void nn_test_random1(nn_t a, rand_t state, len_t m)
{
	int bits_set = (int) randint(m + 1, state);
	const bits_t bits = m*WORD_BITS;
    long i;

	nn_zero(a, m);

	for (i = 0; i < bits_set; i++)
		nn_bit_set(a, randint(bits, state));
}

void nn_test_random2(nn_t a, rand_t state, len_t m)
{
	nn_t b;
   TMP_INIT;
   
   TMP_START;

   b = (nn_t) TMP_ALLOC(m); 
	
	nn_test_random1(a, state, m);
	nn_test_random1(b, state, m);

	nn_sub_m(a, a, b, m);

	TMP_END;
}

void nn_test_random(nn_t a, rand_t state, len_t m)
{
	switch (randint(3, state))
	{
	case 0:
		nn_test_random1(a, state, m);
		break;
	case 1:
		nn_test_random2(a, state, m);
		break;
	case 2:
		nn_random(a, state, m);
		break;
	default:
		printf("Bug in randint\n");
		abort();
	}
}

/**********************************************************************
 
    Printing functions

**********************************************************************/

void nn_printx(nn_src_t a, len_t m)
{
   long i;

   for (i = 0; i < m - 1; i++)
      bsdnt_printf("%wx ", a[i]);

   if (m) bsdnt_printf("%wx", a[m - 1]);
   else bsdnt_printf("%wx", 0);
}

void nn_printx_short(nn_src_t a, len_t m)
{
   if (m < 5) 
	  nn_printx(a, m);
   else
      bsdnt_printf("%wx %wx...%wx %wx", a[0], a[1], a[m - 2], a[m - 1]);
 }

void nn_printx_diff(nn_src_t a, nn_src_t b, len_t m)
{
   len_t i, end = m, start = m;

   for (start = 0; start < m; start++)
      if (a[start] != b[start])
         break;

   for (i = start; i < m; i++)
      if (a[i] != b[i])
         end = i;

   if (start != m)
   {
      bsdnt_printf("diff at word %m (%wx  vs %wx)", start, a[start], b[start]);
      bsdnt_printf(" through word %m (%wx vs %wx)", end, a[end], b[end]);
   } else
      printf("don't differ");
}

/**********************************************************************
 
    Logical bit manipulation functions

**********************************************************************/

#ifndef HAVE_ARCH_nn_bit_set

void nn_bit_set(nn_t a, bits_t b)
{
#if WORD_BITS == 64
	const bits_t bit = b & 63;
	const len_t word = b >> 6;
#else /* WORD_BITS == 32 */
	const bits_t bit = b & 31;
	const len_t word = b >> 5;
#endif

	a[word] |= (WORD(1) << bit);
}

#endif

#ifndef HAVE_ARCH_nn_bit_clear

void nn_bit_clear(nn_t a, bits_t b)
{
#if WORD_BITS == 64
	const bits_t bit = b & 63;
	const len_t word = b >> 6;
#else /* WORD_BITS == 32 */
	const bits_t bit = b & 31;
	const len_t word = b >> 5;
#endif

	a[word] &= ~(WORD(1) << bit);
}

#endif

#ifndef HAVE_ARCH_nn_bit_test

int nn_bit_test(nn_src_t a, bits_t b)
{
#if WORD_BITS == 64
	const bits_t bit = b & 63;
	const len_t word = b >> 6;
#else /* WORD_BITS == 32 */
	const bits_t bit = b & 31;
	const len_t word = b >> 5;
#endif

	return ((a[word] & (WORD(1) << bit)) != WORD(0));
}

#endif

/**********************************************************************
 
    Linear arithmetic functions

**********************************************************************/

#ifndef HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   dword_t t;
   long i;

   ASSERT(ci <= 1);
   
   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] + (dword_t) c[i] + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi)
{
   dword_t t;
   long i;

   ASSERT(bi <= 1);
   
   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] - (dword_t) c[i] - (dword_t) bi;
      a[i] = (word_t) t;
      bi = -(t >> WORD_BITS);
   }

   return bi;
}

#endif

#ifndef HAVE_ARCH_nn_shl_c

word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   dword_t t;
   long i;

   ASSERT(bits < WORD_BITS);
   ASSERT(ci < ((word_t) 1 << bits)); 

   for (i = 0; i < m; i++)
   {
      t = (((dword_t) b[i]) << bits);
      a[i] = (word_t) t + ci;
      ci = (t >> WORD_BITS); 
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_shr_c

word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   dword_t t;
   long i;
   
   ASSERT(bits < WORD_BITS);
   ASSERT((ci << bits) == (word_t) 0); 
   
   bits = WORD_BITS - bits;
   
   for (i = m - 1; i >= 0L; i--)
   {
      t = (((dword_t) b[i]) << bits);
      a[i] = (t >> WORD_BITS) + ci;
      ci = (word_t) t; 
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_add1

word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t c)
{
   dword_t t;
   long i;

   for (i = 0; i < m && c != 0; i++)
   {
      t = (dword_t) b[i] + (dword_t) c;
      a[i] = (word_t) t;
      c = (t >> WORD_BITS);
   }

   if (a != b)
      for ( ; i < m; i++)
         a[i] = b[i];

   return c;
}

#endif

#ifndef HAVE_ARCH_nn_sub1

word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c)
{
   dword_t t;
   long i;

   for (i = 0; i < m && c != 0; i++)
   {
      t = (dword_t) b[i] - (dword_t) c;
      a[i] = (word_t) t;
      c = -(t >> WORD_BITS);
   }

   if (a != b)
      for ( ; i < m; i++)
         a[i] = b[i];

   return c;
}

#endif

#ifndef HAVE_ARCH_nn_neg_c

word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
{
   dword_t t;
   long i;
   
   ASSERT (ci <= 1);
   
   ci = 1 - ci;

   for (i = 0; i < m && ci != 0; i++)
   {
      t = (dword_t) ~b[i] + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   for ( ; i < m; i++)
      a[i] = ~b[i];

   return (word_t) 1 - ci;
}

#endif

#ifndef HAVE_ARCH_nn_mul1_c

word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] * (dword_t) c + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_addmul1_c

word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] + (dword_t) b[i] * (dword_t) c + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_submul1_c

word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] - (dword_t) b[i] * (dword_t) c - (dword_t) ci;
      a[i] = (word_t) t;
      ci = -(t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_simple_c

word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci)
{
   dword_t t;
   long i;

   ASSERT(d != 0);
   ASSERT(ci < d);

   for (i = m - 1; i >= 0; i--)
   {
      t = (((dword_t) ci) << WORD_BITS) + (dword_t) a[i];
      q[i] = t / (dword_t) d;
      ci = (word_t) (t % (dword_t) d);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_preinv_c

word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t dinv, word_t ci)
{
   long i;
   
   ASSERT(ci < d);

   for (i = m - 1; i >= 0; --i)
      divrem21_preinv1(q[i], ci, ci, a[i], d, dinv);

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem_hensel1_preinv_c

word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci)
{
   long i;
   dword_t t, r;
   
   ASSERT(d & (word_t) 1);
   
   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] - (dword_t) ci;
      q[i] = (word_t) t * inv;
      r = (dword_t) q[i] * (dword_t) d;
      ci = (word_t) (r >> WORD_BITS) - (word_t) (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mod1_preinv_c

word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci)
{
   dword_t t, u;
   word_t a1, a0;

   ASSERT(d != 0);
   ASSERT(ci < d);
      
   if (m & 1) /* odd number of words plus carry word */
   {
      a1 = ci;
      a0 = a[--m];
   } else /* even number of words plus carry word */
   {
      if (m == 0) return ci;

      m -= 2;
      t = (dword_t) a[m] + (((dword_t) a[m + 1]) << WORD_BITS);
      u = t + (dword_t) ci * (dword_t) inv.b2;
      if (u < t) u += (dword_t) inv.b2;
      a1 = (word_t) (u >> WORD_BITS);
      a0 = (word_t) u;
   }

   /* reduce to two words */
   while (m >= 2)
   {
      m -= 2;
      u = (dword_t) a[m] + (((dword_t) a[m + 1]) << WORD_BITS);
      t = u + (dword_t) a0 * (dword_t) inv.b2;
      if (t < u) t += (dword_t) inv.b2;
      u = t + (dword_t) a1 * (dword_t) inv.b3;
      if (u < t) u += (dword_t) inv.b2;
      a1 = (word_t) (u >> WORD_BITS);
      a0 = (word_t) u;
   }
   
   /* reduce top word mod d */
   u = (dword_t) a0 + (dword_t) a1 * (dword_t) inv.b1;

   return u % (dword_t) d;
}

#endif

/**********************************************************************
 
    Comparison

**********************************************************************/

#ifndef HAVE_ARCH_nn_cmp_m

int nn_cmp_m(nn_src_t a, nn_src_t b, len_t m)
{
   while (m--)
   {
      if (a[m] != b[m])
      {
         if (a[m] > b[m])
            return 1;
         else
            return -1;
      }
   }

   return 0;
}

#endif

/**********************************************************************

    Bit operations

**********************************************************************/

#ifndef HAVE_ARCH_nn_scan1

bits_t nn_scan1(bits_t skip, nn_src_t a, len_t m)
{
   len_t i = skip / WORD_BITS;
   int rem = skip % WORD_BITS;

   word_t x = a[i] >> rem;
   if (x)
      return skip + low_zero_bits(x);

   for (i++; i < m; i++)
   {
      if (a[i])
         return (bits_t)i * WORD_BITS + low_zero_bits(a[i]);
   }

  return -1;
}

#endif
