/* 
  Copyright (C) 2013 William Hart

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
#define ITER 20000

rand_t state;

int test_mul_kara(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mul_kara...");

   TEST_START(1, ITER) /* test mul_kara gives same as mul_classical */
   {
      do {
         randoms_upto(120, NONZERO, state, &m, NULL);
         randoms_upto(120, NONZERO, state, &n, NULL);
      } while (m < n || n < 2 || n <= (m + 1)/2);

      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, n);
      nn_mul_kara(r2, a, m, b, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mul_toom33(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mul_toom33...");

   TEST_START(1, ITER) /* test mul_toom33 gives same as mul_classical */
   {
      do {
         randoms_upto(200, NONZERO, state, &m, NULL);
         randoms_upto(200, NONZERO, state, &n, NULL);
      } while (m < n || n < 3 || n <= 2*((m + 2)/3));

      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, n);
      nn_mul_toom33(r2, a, m, b, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mul_toom32(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mul_toom32...");

   TEST_START(1, ITER) /* test mul_toom32 gives same as mul_classical */
   {
      do {
         randoms_upto(200, NONZERO, state, &m, NULL);
         randoms_upto(200, NONZERO, state, &n, NULL);
      } while (2*((m + 2)/3) < n || n < 5 || n <= (m + 2)/3);

      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, n);
      nn_mul_toom32(r2, a, m, b, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mulmid_kara_m(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mulmid_kara_m...");

   TEST_START(1, ITER) /* test mulmid_kara and mulmid_classical agree */
   {
      randoms_upto(200, NONZERO, state, &n, NULL);
      n++; /* n is at least 2 */
      randoms_upto(30, ANY, state, &m, NULL);
      m += 2*n - 1; /* m is at least 2*n - 1 */

      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(m - n + 3, ANY, state, &r1, &r2, NULL);
      
      nn_mulmid_classical(r1 + m - n + 1, r1, a, m, b, n);
      nn_mulmid_kara(r2 + m - n + 1, r2, a, m, b, n);
      
      result = nn_equal_m(r1, r2, m - n + 3);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n); 
         print_debug_diff(r1, r2, m - n + 3);
      }
   } TEST_END;

   return result;
}

int test_mullow_kara_m(void)
{
   int result = 1;
   len_t n;
   nn_t a, b, r1, r2;
   
   printf("mullow_kara_m...");

   TEST_START(1, ITER) /* test mullow_kara is the same as mullow_classical */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      n++; /* n at least 2 */

      randoms_of_len(n, ANY, state, &a, &b, NULL);
      randoms_of_len(n + 2, ANY, state, &r1, &r2, NULL);
      
      nn_mullow_classical(r1 + n, r1, a, n, b, n);
      nn_mullow_kara_m(r2 + n, r2, a, b, n);
      
      result = (nn_equal_m(r1, r2, n + 2));

      if (!result) 
      {
         print_debug(a, n); print_debug(b, n);
         print_debug_diff(r1, r2, n + 2);
      }
   } TEST_END;

   return result;
}

int test_mullow_kara(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;
   
   printf("mullow_kara...");

   TEST_START(1, ITER) /* test mullow_kara is the same as mullow_classical */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      randoms_upto(150, ANY, state, &m, NULL);
      m += n;
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(m + 2, ANY, state, &r1, &r2, NULL);
      
      nn_mullow_classical(r1 + m, r1, a, m, b, n);
      nn_mullow_kara(r2 + m, r2, a, m, b, n);
      
      result = (nn_equal_m(r1, r2, m + 2));

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + 2);
      }
   } TEST_END;

   return result;
}

int test_divapprox_divconquer_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, s, q1, q2, d;
   preinv2_t inv;
   
   printf("divapprox_divconquer_preinv...");

   TEST_START(1, ITER) /* test divapprox is at most one more than divrem */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
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

      inv = precompute_inverse2(d[n - 1], d[n - 2]);
      nn_divrem_classical_preinv_c(q1, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      nn_divapprox_divconquer_preinv_c(q2, r2, m + n - 1, d, n, inv, r2[m + n - 1]);

      result = (nn_cmp_m(q1, q2, m) >= 0);
      nn_add1(q1, q1, m, 1);
      result |= (nn_cmp_m(q1, q2, m) >= 0);
      nn_sub1(q1, q1, m, 1);
      if (result)
      {
         result = (nn_cmp_m(q1, q2, m) <= 0);
         nn_add1(q1, q1, m, 1);
         nn_add1(q2, q2, m, 2);
         result |= (nn_cmp_m(q1, q2, m) <= 0);
         nn_sub1(q1, q1, m, 1);
         nn_sub1(q2, q2, m, 2);
      }  
      
      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
      }
   } TEST_END;

   return result;
}

int test_div_divconquer_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, s, q1, q2, d;
   preinv2_t inv;
   
   printf("div_divconquer_preinv...");

   TEST_START(1, ITER) /* test div is the same as divrem */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
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

      inv = precompute_inverse2(d[n - 1], d[n - 2]);
      nn_divrem_classical_preinv_c(q1, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      nn_div_divconquer_preinv_c(q2, r2, m + n - 1, d, n, inv, r2[m + n - 1]);

      result = (nn_cmp_m(q1, q2, m) == 0);
      
      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
      }
   } TEST_END;

   return result;
}

int test_divrem_divconquer_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, s, q1, q2, d;
   preinv2_t inv;
   
   printf("divrem_divconquer_preinv...");

   TEST_START(1, ITER) /* test divrem is the same as classical divrem */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
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

      inv = precompute_inverse2(d[n - 1], d[n - 2]);
      nn_divrem_classical_preinv_c(q1, r1, m + n - 1, d, n, inv, r1[m + n - 1]);

      nn_divrem_divconquer_preinv_c(q2, r2, m + n - 1, d, n, inv, r2[m + n - 1]);

      result = (nn_equal_m(q1, q2, m)); 
      result &= (nn_equal_m(r1, r2, n));

      if (!result) 
      {
         bsdnt_printf("inv = %wx\n", inv);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
         print_debug_diff(r1, r2, n);
      }
   } TEST_END;

   return result;
}

int test_subquadratic(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_mul_kara);
   RUN(test_mul_toom33);
   RUN(test_mul_toom32);
   RUN(test_mulmid_kara_m);
   RUN(test_mullow_kara_m);
   RUN(test_mullow_kara);
   RUN(test_divapprox_divconquer_preinv);
   RUN(test_div_divconquer_preinv);
   RUN(test_divrem_divconquer_preinv);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting nn_subquadratic functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_subquadratic();

   randclear(state);

   return ret;
}

