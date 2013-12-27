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
#define ITER 10000

rand_t state;

int test_mul_m(void)
{
   int result = 1;
   len_t m;
   nn_t a, b, r1, r2;

   printf("mul_m...");

   TEST_START(1, ITER) /* test mul_m gives same as mul_classical */
   {
      randoms_upto(300, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      
      randoms_of_len(2*m, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, m);
      nn_mul_m(r2, a, b, m);
      
      result = nn_equal_m(r1, r2, 2*m);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m);
         print_debug_diff(r1, r2, 2*m);
      }
   } TEST_END;

   return result;
}

int test_mul(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mul...");

   TEST_START(1, ITER) /* test mul gives same as mul_classical */
   {
      randoms_upto(300, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m, b, n);
      nn_mul(r2, a, m, b, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mullow_m(void)
{
   int result = 1;
   len_t n;
   nn_t a, b, r1, r2;
   
   printf("mullow_m...");

   TEST_START(1, ITER) /* test mullow_kara is the same as mullow_classical */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      
      randoms_of_len(n, ANY, state, &a, &b, NULL);
      randoms_of_len(n + 2, ANY, state, &r1, &r2, NULL);
      
      nn_mullow_classical(r1 + n, r1, a, n, b, n);
      nn_mullow_m(r2 + n, r2, a, b, n);
      
      result = (nn_equal_m(r1, r2, n + 2));

      if (!result) 
      {
         print_debug(a, n); print_debug(b, n);
         print_debug_diff(r1, r2, n + 2);
      }
   } TEST_END;

   return result;
}

int test_mullow(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;
   
   printf("mullow...");

   TEST_START(1, ITER) /* test mullow_kara is the same as mullow_classical */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      randoms_upto(150, ANY, state, &m, NULL);
      m += n;
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(m + 2, ANY, state, &r1, &r2, NULL);
      
      nn_mullow_classical(r1 + m, r1, a, m, b, n);
      nn_mullow(r2 + m, r2, a, m, b, n);
      
      result = (nn_equal_m(r1, r2, m + 2));

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n);
         print_debug_diff(r1, r2, m + 2);
      }
   } TEST_END;

   return result;
}

int test_divrem(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, q, d;
   
   printf("divrem...");

   TEST_START(1, ITER) /* test a = d*q + r */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      randoms_upto(150, ANY, state, &m, NULL);
      m += n; /* m >= n */

      randoms_of_len(m - n + 1, ANY, state, &q, NULL);
      randoms_of_len(m, ANY, state, &a, &r1, &r2, NULL);
      randoms_of_len(n, FULL, state, &d, NULL);

      nn_copy(r2, a, m);

      nn_divrem(q, a, m, d, n);

      if (m - n >= n) nn_mul(r1, q, m - n, d, n);
      else if (m > n) nn_mul(r1, d, n, q, m - n);
      else nn_zero(r1, m);

      nn_addmul1(r1 + m - n, d, n, q[m - n]);

      nn_add(r1, r1, m, a, n);

      result &= (nn_equal_m(r1, r2, m));

      if (!result) 
      {
         print_debug(a, n); print_debug(q, m - n); print_debug(d, n);
         print_debug_diff(r1, r2, m);
      }
   } TEST_END;

   return result;
}

int test_div(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, q, d;
   
   printf("div...");

   TEST_START(1, ITER) /* test a - d*q < d */
   {
      randoms_upto(150, NONZERO, state, &n, NULL);
      randoms_upto(150, ANY, state, &m, NULL);
      m += n; /* m >= n */

      randoms_of_len(m - n + 1, ANY, state, &q, NULL);
      randoms_of_len(m, ANY, state, &a, &r1, &r2, NULL);
      randoms_of_len(n, FULL, state, &d, NULL);

      nn_copy(r2, a, m);

      nn_divrem(q, a, m, d, n);

      if (m - n >= n) nn_mul(r1, q, m - n, d, n);
      else if (m > n) nn_mul(r1, d, n, q, m - n);
      else nn_zero(r1, m);

      nn_addmul1(r1 + m - n, d, n, q[m - n]);
      nn_sub_m(r2, r2, r1, m);

      result &= (nn_cmp_m(r2, d, n) < 0);

      if (!result) 
      {
         print_debug(a, n); print_debug(q, m - n); print_debug(d, n);
         print_debug(r2, n);
      }
   } TEST_END;

   return result;
}

int test(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_mul_m);
   RUN(test_mul);
   RUN(test_mullow_m);
   RUN(test_mullow);
   RUN(test_divrem);
   RUN(test_div);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting nn functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test();

   randclear(state);

   return ret;
}

