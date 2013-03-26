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

#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "test.h"

#undef ITER
#define ITER 30000

rand_t state;

int test_mul_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, c, r1, r2, s;

   printf("mul_classical...");

   TEST_START(1, ITER) /* test a * (b + c) = a * b + a * c */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(30, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n + 1, ANY, state, &s, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      do {
         randoms_of_len(n, ANY, state, &b, &c, NULL);
         s[n] = nn_add_m(s, b, c, n);
      } while (s[n]);
      
      nn_mul_classical(r1, a, m, b, n);
      nn_mul_classical(r2, a, m, c, n);
      nn_add_m(r2, r2, r1, m + n);

      nn_mul_classical(r1, a, m, s, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n); print_debug(c, n);  print_debug(s, n + 1);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mulmid_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, a1, a2, b1, b2, b, r1, r2, r3, r4;

   printf("mulmid_classical...");

   TEST_START(1, ITER) /* test mulmid_add_rfix */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      n++;

      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n + 1, ANY, state, &b1, &b2, &b, NULL);
      randoms_of_len(m - n + 3, ANY, state, &r1, &r2, &r3, &r4, NULL);
      
      nn_mulmid_classical(r1 + m - n + 1, r1, a, m, b1 + 1, n);
      nn_mulmid_classical(r2 + m - n + 1, r2, a, m, b2 + 1, n);
      _nn_mulmid_add_rfix(b + 1, r4 + m - n + 1, r4, a, m, b1 + 1, b2 + 1, n, b1[0] + b2[0] < b1[0]);
      nn_mulmid_classical(r3 + m - n + 1, r3, a, m, b + 1, n);
      
      nn_add_m(r2, r2, r1, m - n + 3);
      nn_add_m(r3, r3, r4, m - n + 3);

      result = nn_equal_m(r2, r3, m - n + 3);

      if (!result) 
      {
         print_debug(a, m); print_debug(b1, n + 1); print_debug(b2, n + 1); 
         print_debug(r4, m); print_debug_diff(r2, r3, m - n + 2);
      }
   } TEST_END;

   TEST_START(2, ITER) /* test mulmid_add_lfix */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      m++;
      randoms_upto(m, NONZERO, state, &n, NULL);
      n++;

      randoms_of_len(m + 1, ANY, state, &a1, &a2, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(m - n + 3, ANY, state, &r1, &r2, &r3, &r4, NULL);
      
      nn_mulmid_classical(r1 + m - n + 1, r1, a1 + 1, m, b, n);
      nn_mulmid_classical(r2 + m - n + 1, r2, a2 + 1, m, b, n);
      _nn_mulmid_add_lfix(a + 1, r4 + m - n + 1, r4, a1 + 1, a2 + 1, m, b, n, a1[0] + a2[0] < a1[0]);
      nn_mulmid_classical(r3 + m - n + 1, r3, a + 1, m, b, n);
      
      nn_add_m(r2, r2, r1, m - n + 3);
      nn_add_m(r3, r3, r4, m - n + 3);

      result = nn_equal_m(r2, r3, m - n + 3);

      if (!result) 
      {
         print_debug(a1, m + 1); print_debug(a2, m + 1); print_debug(b, n); 
         print_debug(r4, m + 1); print_debug_diff(r2, r3, m - n + 3);
      }
   } TEST_END;

   return result;
}

int test_divrem_classical_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, s, q, d;
   preinv1_t inv;

   printf("divrem_classical_preinv...");

   TEST_START(1, ITER) /* test (a * d) / d = a remainder 0 */
   {
      randoms_upto(30, NONZERO, state, &m, &n, NULL);
      n++; /* require n >= 2 */

      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(n, NORMALISED, state, &d, NULL);
      randoms_of_len(m + n, ANY, state, &r1, NULL);
      
      if (m >= n) nn_mul_classical(r1, a, m, d, n);
      else nn_mul_classical(r1, d, n, a, m);

	   inv = precompute_inverse1(d[n - 1]);
      nn_divrem_classical_preinv_c(q, r1, m + n - 1, d, n, inv, r1[m + n - 1]);
	   
      result = (nn_equal_m(q, a, m) && nn_normalise(r1, n) == 0);

      if (!result) 
      {
         print_debug(a, m); print_debug(q, m); print_debug(d, n);  print_debug(r1, n);
         print_debug_diff(q, a, m);
      }
   } TEST_END;
   
   TEST_START(2, ITER) /* test (a * d + s) / d = a remainder s */
   {
      randoms_upto(30, NONZERO, state, &n, NULL);
      n++; /* require n at least 2 */
      randoms_upto(n + 1, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(m + n, ANY, state, &r1, NULL);
      
      /* ensure s is reduced mod d */
      do {
         randoms_of_len(n, NORMALISED, state, &d, NULL);
         randoms_of_len(n, ANY, state, &s, NULL);
      } while (nn_cmp_m(d, s, n) <= 0);
      
      nn_mul_classical(r1, d, n, a, m);
      nn_add(r1, r1, n + m, s, n);
      
      inv = precompute_inverse1(d[n - 1]);
      nn_divrem_classical_preinv_c(q, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      result = (nn_equal_m(q, a, m) && nn_equal_m(s, r1, n));

      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q, a, m);
         print_debug_diff(s, r1, m);
      }
   } TEST_END;

   return result;
}

int test_divapprox_classical_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, s, q1, q2, d, t;
   preinv1_t inv;
   
   printf("divapprox_classical_preinv...");

   TEST_START(1, ITER) /* test divapprox is at most one more than divrem */
   {
      randoms_upto(30, NONZERO, state, &n, NULL);
      n++; /* require n at least 2 */
      randoms_upto(n + 1, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q1, &q2, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      /* ensure s is reduced mod d */
      do {
         randoms_of_len(n, NORMALISED, state, &d, NULL);
         randoms_of_len(n, ANY, state, &s, NULL);
      } while (nn_cmp_m(d, s, n) <= 0);
      
      nn_mul_classical(r1, d, n, a, m);
      nn_add(r1, r1, m + n, s, n);
      nn_copy(r2, r1, m + n);

      inv = precompute_inverse1(d[n - 1]);
      nn_divrem_classical_preinv_c(q1, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      nn_divapprox_classical_preinv_c(q2, r2, m + n - 1, d, n, inv, r2[m + n - 1]);

      result = nn_equal_m(q1, q2, m);
      nn_add1(q1, q1, m, 1);
      result |= nn_equal_m(q1, q2, m);

      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
      }
   } TEST_END;

   TEST_START(2, ITER) /* test divapprox and mullow give divrem */
   {
      randoms_upto(30, NONZERO, state, &n, NULL);
      n += 2;
      randoms_upto(n - 1, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q1, &q2, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      /* ensure s is reduced mod d */
      do {
         randoms_of_len(n, NORMALISED, state, &d, NULL);
         randoms_of_len(n, ANY, state, &s, NULL);
      } while (nn_cmp_m(d, s, n) <= 0);
      
      randoms_of_len(n, ANY, state, &t, NULL);

      nn_mul_classical(r1, d, n, a, m);
      nn_add(r1, r1, m + n, s, n);
      nn_copy(r2, r1, m + n);

      inv = precompute_inverse1(d[n - 1]);
      nn_divrem_classical_preinv_c(q1, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      nn_divapprox_classical_preinv_c(q2, r2, m + n - 1, d, n, inv, r2[m + n - 1]);
      nn_mullow_classical(t + n - 2, t, d, n - 2, q2, m);
      nn_sub_m(r2, r2, t, n);
      if (!nn_equal_m(q1, q2, m)) 
      {
         nn_sub1(q2, q2, m, 1);
         nn_add_m(r2, r2, d, n);
      }

      result  = nn_equal_m(q1, q2, m);
      result &= nn_equal_m(r1, r2, n);
      
      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
      }
   } TEST_END;

   return result;
}

int test_mullow_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2, ov;
   
   printf("mullow_classical...");

   TEST_START(1, ITER) /* test a * (b + c) = a * b + a * c */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, n);
      
      nn_mullow_classical(ov, r2, a, m, b, n);
      nn_mulhigh_classical(r2 + m, a, m, b, n, ov);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         bsdnt_printf("ov[0] = %wx, ov[1] = %wx\n", ov[0], ov[1]);
         print_debug(a, m); print_debug(b, n); 
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_div_hensel_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, a1, a2, r1, q, q1, q2, d, ov, ov2, t;
   hensel_preinv1_t inv;
   
   printf("div_hensel_preinv...");

   TEST_START(1, ITER) /* test (a * d) / d = a */
   {
      randoms_upto(30, NONZERO, state, &m, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ODD, state, &d, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &q, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      
      if (m >= n) nn_mul_classical(r1, a, m, d, n);
      else nn_mul_classical(r1, d, n, a, m);

	  precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q, r1, m + n, d, n, inv);

      result = (nn_equal_m(q, a, m) && nn_normalise(q, m + n) == nn_normalise(a, m));

      if (!result)
	  {
		  bsdnt_printf("m = %m, n = %m\n", m, n);
		  print_debug(a, m); print_debug(q, m); print_debug(d, n); 
		  print_debug_diff(q, a, m);
	  }
          
   } TEST_END;

   TEST_START(2, ITER) /* test inexact division yields correct overflow */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(n, ODD, state, &d, NULL);
      randoms_of_len(2, ANY, state, &ov, &ov2, NULL);
      
      precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q, a, m, d, n, inv);

      nn_mullow_classical(ov2, a, q, m, d, n);

      result = (nn_equal_m(ov, ov2, 2));

      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q, m); print_debug(d, n); 
         print_debug_diff(ov, ov2, 2L);
      }
   } TEST_END;

   TEST_START(chaining, ITER) /* test chaining of div_hensel */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(2*m, ANY, state, &a1, &a2, &q, NULL);
      randoms_of_len(2*m, ANY, state, &q1, &q2, NULL);
      randoms_of_len(n, ODD, state, &d, &t, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      
      nn_copy(a2, a1, 2*m);

      precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q1, a1, m, d, n, inv);
      nn_mulhigh_classical(t, q1, m, d, n, ov);
      nn_sub(a1 + m, a1 + m, m, t, n);
      nn_div_hensel_preinv(ov, q1 + m, a1 + m, m, d, n, inv);

      nn_div_hensel_preinv(ov, q2, a2, 2*m, d, n, inv);

      result = (nn_equal_m(q1, q2, 2*m));

      if (!result) 
      {
         bsdnt_printf("inv = %wx, ov[0] = %wx, ov[1] = %wx\n", inv, ov[0], ov[1]);
         print_debug(a1, 2*m); print_debug(a2, 2*m); print_debug(q, 2*m); 
         print_debug(q1, 2*m); print_debug(q2, 2*m); print_debug(d, n); print_debug(t, n); 
         print_debug_diff(q1, q2, 2*m);
      }
   } TEST_END;

   return result;
}

int test_quadratic(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_mul_classical);
   RUN(test_mullow_classical);
   RUN(test_mulmid_classical);
   RUN(test_divrem_classical_preinv);
   RUN(test_divapprox_classical_preinv);
   RUN(test_div_hensel_preinv);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting nn_quadratic functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_quadratic();

   randclear(state);

   return ret;
}

