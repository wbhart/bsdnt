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

rand_t state;

#undef ITER
#define ITER 50000

int test_gc(void)
{
   int result = 1;
   len_t m;
   word_t w1, w2, w3, w4;
   nn_t a, b, c;

   printf("gc...");

   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &w1, &w2, &w3, &w4, NULL);
      
      randoms_of_len(w1, ANY, state, &a, &b, &c, NULL);
   } TEST_END;

   return result;
}

int test_tmp(void)
{
   int result = 1;
   len_t m1, m2, m3;
   nn_t a, b, c;
   TMP_INIT;

   printf("tmp...");

   TEST_START(1, 64) 
   {
      TMP_START;
      
      randoms_upto(2048, NONZERO, state, &m1, &m2, &m3, NULL);
      
      a = (nn_t) TMP_ALLOC(m1);
      b = (nn_t) TMP_ALLOC(m2);
      c = (nn_t) TMP_ALLOC(m3);

      randoms_of_len(m1, ANY, state, &a, NULL);
      randoms_of_len(m2, ANY, state, &b, NULL);
      randoms_of_len(m3, ANY, state, &c, NULL);

      TMP_END;
   } TEST_END;

   return result;
}

int test_bit_set(void)
{
   int result = 1;
   nn_t a;
   len_t m;
   bits_t bit, sh1;
   
   printf("nn_bit_set...");

   /* test that setting a bit works */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m*WORD_BITS, ANY, state, &bit, NULL);

      randoms_of_len(m, ANY, state, &a, NULL);
      
      nn_bit_set(a, bit);

      result = nn_bit_test(a, bit);

      if (!result) 
      {
         bsdnt_printf("bit = %b\n", bit);
		 print_debug(a, m);  
      }
   } TEST_END;

   /* test that setting a bit then shifting works */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m*WORD_BITS - WORD_BITS/2, ANY, state, &bit, NULL);
      randoms_upto(WORD_BITS/2, ANY, state, &sh1, NULL);

      randoms_of_len(m, ANY, state, &a, NULL);
      
      nn_bit_set(a, bit);
      nn_shl(a, a, m, sh1);

      result = nn_bit_test(a, bit + sh1);

      if (!result) 
      {
         bsdnt_printf("bit = %b, sh1 = %b\n", bit, sh1);
		 print_debug(a, m);  
      }
   } TEST_END;

   return result;
}

int test_bit_clear(void)
{
   int result = 1;
   nn_t a;
   len_t m;
   bits_t bit, sh1;
   
   printf("nn_bit_clear...");

   /* test that setting a bit works */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m*WORD_BITS, ANY, state, &bit, NULL);

      randoms_of_len(m, ANY, state, &a, NULL);
      
      nn_bit_clear(a, bit);

      result = !nn_bit_test(a, bit);

      if (!result) 
      {
         bsdnt_printf("bit = %b\n", bit);
		 print_debug(a, m);  
      }
   } TEST_END;

   /* test that setting a bit then shifting works */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m*WORD_BITS - WORD_BITS/2, ANY, state, &bit, NULL);
      randoms_upto(WORD_BITS/2, ANY, state, &sh1, NULL);

      randoms_of_len(m, ANY, state, &a, NULL);
      
      nn_bit_clear(a, bit);
      nn_shl(a, a, m, sh1);

      result = !nn_bit_test(a, bit + sh1);

      if (!result) 
      {
         bsdnt_printf("bit = %b, sh1 = %b\n", bit, sh1);
		 print_debug(a, m);  
      }
   } TEST_END;

   return result;
}

int test_add_m(void)
{
   int result = 1;
   nn_t a, b, c, r1, r2;
   len_t m, n;
   word_t ci;

   printf("nn_add_m...");

   /* test (a + b) + c = a + (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, &c, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      r1[m]  = nn_add_m(r1, a, b, m);
      r1[m] += nn_add_m(r1, r1, c, m);

      r2[m]  = nn_add_m(r2, b, c, m);
      r2[m] += nn_add_m(r2, r2, a, m);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); print_debug(c, m); 
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of addition */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);

      randoms_of_len(m + n, ANY, state, &a, &b, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
       
      ci = nn_add_m(r1, a, b, m);
      r1[m + n] = nn_add_mc(r1 + m, a + m, b + m, n, ci);

      r2[m + n] = nn_add_m(r2, a, b, m + n);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m n = %m\n", m, n);
         print_debug(a, m + n); print_debug(b, m + n);
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_add(void)
{
   int result = 1;
   nn_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   word_t ci;

   printf("nn_add...");

   /* test (a + b) + c = (a + c) + b */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, &m3, NULL);
      
      randoms_of_len(m1, ANY, state, &a, NULL);
      randoms_of_len(m2, ANY, state, &b, NULL);
      randoms_of_len(m3, ANY, state, &c, NULL);    
      randoms_of_len(m1 + 1, ANY, state, &r1, &r2, NULL);
      
      r1[m1]  = nn_add(r1, a, m1, b, m2);
      r1[m1] += nn_add(r1, r1, m1, c, m3);

      r2[m1]  = nn_add(r2, a, m1, c, m3);
      r2[m1] += nn_add(r2, r2, m1, b, m2);

      result = nn_equal_m(r1, r2, m1 + 1);

      if (!result) 
      {
         print_debug(a, m1); print_debug(b, m2); print_debug(c, m3);
         print_debug_diff(r1, r2, m1 + 1);
      }
   } TEST_END;

   /* test chaining of addition */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m1, &m2, &m3, NULL);
      
      randoms_of_len(m1 + m2 + m3, ANY, state, &a, NULL);
      randoms_of_len(m2 + m3, ANY, state, &b, NULL);
      randoms_of_len(m1 + m2 + m3 + 1, ANY, state, &r1, &r2, NULL);
      
      ci = nn_add(r1, a, m3, b, m3);
      r1[m1 + m2 + m3] = nn_add_c(r1 + m3, a + m3, m1 + m2, b + m3, m2, ci);

      r2[m1 + m2 + m3] = nn_add(r2, a, m1 + m2 + m3, b, m2 + m3);
      
      result = nn_equal_m(r1, r2, m1 + m2 + m3 + 1);

      if (!result) 
      {
         bsdnt_printf("m1 = %m, m2 = %m, m3 = %m\n", m1, m2, m3);
         print_debug(a, m1 + m2 + m3); print_debug(b, m2 + m3); 
         print_debug_diff(r1, r2, m1 + m2 + m3 + 1);
      }
   } TEST_END;

   return result;
}

int test_sub_m(void)
{
   int result = 1;
   nn_t a, b, c, r1, r2;
   len_t m, n;
   word_t ci;

   printf("nn_sub_m...");

   /* test (a - b) - c = (a - c) - b */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, &c, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      r1[m] = -nn_sub_m(r1, a, b, m);
      r1[m] -= nn_sub_m(r1, r1, c, m);

      r2[m] = -nn_sub_m(r2, a, c, m);
      r2[m] -= nn_sub_m(r2, r2, b, m);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); print_debug(c, m); 
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of subtraction */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);

      randoms_of_len(m + n, ANY, state, &a, &b, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
       
      ci = nn_sub_m(r1, a, b, m);
      r1[m + n] = -nn_sub_mc(r1 + m, a + m, b + m, n, ci);

      r2[m + n] = -nn_sub_m(r2, a, b, m + n);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m\n", m, n);
         print_debug(a, m + n); print_debug(b, m + n);
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, NULL);
      
      r1[m]  = nn_add_m(r1, a, b, m);
      r1[m] -= nn_sub_m(r1, r1, b, m);

      result = (nn_equal_m(r1, a, m) && (r1[m] == 0));

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); 
         print_debug_diff(r1, a, m);
         bsdnt_printf("r1[%m] = %wx\n", m, r1[m]); 
      }
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(3, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      r1[m] = -nn_neg(r1, b, m);
      r1[m] += nn_add_m(r1, r1, a, m);
     
      r2[m] = -nn_sub_m(r2, a, b, m);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); 
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   return result;
}

int test_sub(void)
{
   int result = 1;
   nn_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   word_t ci;

   printf("nn_sub...");

   /* test (a - b) - c = (a - c) - b */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, &m3, NULL);
      
      randoms_of_len(m1, ANY, state, &a, NULL);
      randoms_of_len(m2, ANY, state, &b, NULL);
      randoms_of_len(m3, ANY, state, &c, NULL);    
      randoms_of_len(m1 + 1, ANY, state, &r1, &r2, NULL);
      
      r1[m1] = -nn_sub(r1, a, m1, b, m2);
      r1[m1] -= nn_sub(r1, r1, m1, c, m3);

      r2[m1] = -nn_sub(r2, a, m1, c, m3);
      r2[m1] -= nn_sub(r2, r2, m1, b, m2);

      result = nn_equal_m(r1, r2, m1 + 1);

      if (!result) 
      {
         print_debug(a, m1); print_debug(b, m2); print_debug(c, m3);
         print_debug_diff(r1, r2, m1 + 1);
      }
   } TEST_END;

   /* test chaining of subtraction */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m1, &m2, &m3, NULL);
      
      randoms_of_len(m1 + m2 + m3, ANY, state, &a, NULL);
      randoms_of_len(m2 + m3, ANY, state, &b, NULL);
      randoms_of_len(m1 + m2 + m3 + 1, ANY, state, &r1, &r2, NULL);
      
      ci = nn_sub(r1, a, m3, b, m3);
      r1[m1 + m2 + m3] = -nn_sub_c(r1 + m3, a + m3, m1 + m2, b + m3, m2, ci);

      r2[m1 + m2 + m3] = -nn_sub(r2, a, m1 + m2 + m3, b, m2 + m3);
      
      result = nn_equal_m(r1, r2, m1 + m2 + m3 + 1);

      if (!result) 
      {
         bsdnt_printf("m1 = %m, m2 = %m, m3 = %m\n", m1, m2, m3);
         print_debug(a, m1 + m2 + m3); print_debug(b, m2 + m3); 
         print_debug_diff(r1, r2, m1 + m2 + m3 + 1);
      }
   } TEST_END;

   return result;
}

int test_shl(void)
{
   int result = 1;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m, n;
   word_t ci;

   printf("nn_shl...");

   /* test (a << sh1) << sh2 = (a << sh2) << sh1 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      randoms_upto(WORD_BITS - sh1, ANY, state, &sh2, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
               
      r1[m] = nn_shl(r1, a, m, sh1);
      nn_shl(r1, r1, m + 1, sh2);

      r2[m] = nn_shl(r2, a, m, sh2);
      nn_shl(r2, r2, m + 1, sh1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, sh1 = %b, sh2 = %b\n", m, sh1, sh2);
         print_debug(a, m); 
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of shl */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
         
      ci = nn_shl(r1, a, m, sh1);
      r1[m + n] = nn_shl_c(r1 + m, a + m, n, sh1, ci);
      
      r2[m + n] = nn_shl(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, sh1 = %b\n", m, n, sh1);
         print_debug(a, m + n); 
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   /* test a << 1 = a + a */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
         
      r1[m] = nn_shl(r1, a, m, 1);
      
      r2[m] = nn_add_m(r2, a, a, m);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         print_debug(a, m); 
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   return result;
}

int test_shr(void)
{
   int result = 1;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m, n;
   word_t ci;

   printf("nn_shr...");

   /* test (a >> sh1) >> sh2 = (a >> sh2) >> sh1 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      randoms_upto(WORD_BITS - sh1, ANY, state, &sh2, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, &r2, NULL);
               
      nn_shr(r1, a, m, sh1);
      nn_shr(r1, r1, m, sh2);

      nn_shr(r2, a, m, sh2);
      nn_shr(r2, r2, m, sh1);

      result = nn_equal_m(r1, r2, m);

      if (!result) 
      {
         print_debug(a, m); 
         print_debug_diff(r1, r2, m);
      }
   } TEST_END;
   
   /* test chaining of shr */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, &r1, &r2, NULL);

      ci = nn_shr(r1 + n, a + n, m, sh1);
      nn_shr_c(r1, a, n, sh1, ci);
      
      nn_shr(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, sh1 = %b\n", m, n, sh1);
         print_debug(a, m + n); 
         print_debug_diff(r1, r2, m + n + 1);
      }
   }  TEST_END;
   
   /* test (a << sh1) >> sh1 = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r2, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, NULL);
      
      r1[m] = nn_shl(r1, a, m, sh1);
      
      nn_shr_c(r2, r1, m, sh1, sh1 == 0 ? 0 : r1[m] << (WORD_BITS - sh1));
      
      result = nn_equal_m(a, r2, m);

      if (!result) 
      {
         bsdnt_printf("m = %m, sh1 = %b\n", m, sh1);
         print_debug(a, m); print_debug(r1, m + 1); 
         print_debug_diff(a, r2, m);
      }
   }  TEST_END;

   return result;
}

int test_copy(void)
{
   int result = 1;
   nn_t a, r1;
   len_t m;

   printf("nn_copy...");

   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
         
      nn_copy(r1, a, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result) 
      {
         print_debug(a, m); 
         print_debug_diff(a, r1, m);
      }
   } TEST_END;

   return result;
}

int test_equal_m(void)
{
   int result = 1;
   nn_t a, r1;
   len_t m, s;

   printf("nn_equal_m...");

   /* test copying and then modifiying yields non-equal integer */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &s, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
      
      nn_copy(r1, a, m);
      a[s] += 1;

      result = !nn_equal_m(r1, a, m);

      if (!result) 
      {
         print_debug(a, m); 
         print_debug_diff(a, r1, m);
      }
   } TEST_END;

   return result;
}

int test_equal(void)
{
   int result = 1;
   nn_t a, b, r1, r2;
   len_t m1, m2;
   
   printf("nn_equal...");

   /* test that equal things compare equal */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = nn_equal(r1, m1, r2, m1);

      if (!result) 
      {
         print_debug(r1, m1); print_debug(r2, m1);
         print_debug_diff(r1, r2, m1);
      }
   } TEST_END;

   /* test that not equal lengths compare not equal */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, NULL);
      
      randoms_of_len(m1, FULL, state, &r1, NULL);
      randoms_of_len(m2, FULL, state, &r2, NULL);
            
      result = !nn_equal(r1, m1, r2, m2);

      if (!result) 
      {
         print_debug(r1, m1); print_debug(r2, m2);
      }
   } TEST_END;

   /* test that not equal values compare not equal */
   TEST_START(3, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         r1[m1] = nn_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = !nn_equal(r1, m1, a, m1);

      if (!result) 
      {
         print_debug(r1, m1); print_debug(a, m1);  print_debug(b, m1);
         print_debug_diff(r1, a, m1);
      }
   } TEST_END;

   return result;
}

int test_zero(void)
{
   int result = 1;
   nn_t a;
   len_t m;

   printf("nn_zero...");

   /* test zeroing */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
         
      nn_zero(a, m);

      result = (nn_normalise(a, m) == 0);

      if (!result) 
      {
         print_debug(a, m); 
      }
   } TEST_END;

   return result;
}

int test_normalise(void)
{
   int result = 1;
   nn_t a, r1;
   len_t m, s1, s2;

   printf("nn_normalise...");

   /* test normalising then copying normalised part yields same integer */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &s1, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
      
      nn_zero(a + s1, m - s1);
      s2 = nn_normalise(a, m);

      nn_zero(r1, m);
      nn_copy(r1, a, s2);

      result = ((s1 >= s2) && ((s2 == 0) || (a[s2 - 1] != 0))
             && nn_equal_m(a, r1, m));

      if (!result) 
      {
         bsdnt_printf("m = %m, s1 = %m, s2 = %m\n", m, s1, s2);
         print_debug(r1, m); print_debug(a, m);
         print_debug_diff(r1, a, m);
      }
   } TEST_END;

   return result;
}

int test_mul1(void)
{
   int result = 1;
   nn_t a, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_mul1...");

   /* test a * (c1 + c2) = a * c1 + a * c2 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &t1, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      t1[m] = nn_mul1(t1, a, m, c1);
      r1[m] = nn_mul1(r1, a, m, c2);
      nn_add_m(r1, r1, t1, m + 1);
      
      r2[m] = nn_mul1(r2, a, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx, c2 = %wx\n", m, c1, c2);
         print_debug(a, m);  print_debug(t1, m + 1);
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of mul1 */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_mul1(r1, a, m, c1);
      r1[m + n] = nn_mul1_c(r1 + m, a + m, n, c1, ci);
      
      r2[m + n] = nn_mul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, c1 = %wx\n", m, n, c1);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_addmul1(void)
{
   int result = 1;
   nn_t a, b, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_addmul1...");

   /* test a + b * (c1 + c2) = a + b * c1 + b * c2 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      r1[m]  = nn_addmul1(r1, b, m, c1);
      r1[m] += nn_addmul1(r1, b, m, c2);
      
      r2[m]  = nn_addmul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx, c2 = %wx\n", m, c1, c2);
         print_debug(a, m); print_debug(b, m);  
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of addmul1 */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m + n);

      randoms(ANY, state, &c1, NULL);

      ci = nn_addmul1(r1, a, m, c1);
      r1[m + n] = nn_addmul1_c(r1 + m, a + m, n, c1, ci);
      
      r2[m + n] = nn_addmul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, c1 = %wx\n", m, n, c1);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_submul1(void)
{
   int result = 1;
   nn_t a, b, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_submul1...");

   /* test a - b * (c1 + c2) = a - b * c1 - b * c2 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      r1[m] = -nn_submul1(r1, b, m, c1);
      r1[m] -= nn_submul1(r1, b, m, c2);
      
      r2[m] = -nn_submul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx, c2 = %wx\n", m, c1, c2);
         print_debug(a, m); print_debug(b, m);  
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of submul1 */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m + n);

      randoms(ANY, state, &c1, NULL);

      ci = nn_submul1(r1, a, m, c1);
      r1[m + n] = -nn_submul1_c(r1 + m, a + m, n, c1, ci);
      
      r2[m + n] = -nn_submul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, c1 = %wx\n", m, n, c1);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_add1(void)
{
   int result = 1;
   nn_t a, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_add1...");

   /* test a + c1 + c2 = a + c2 + c1 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, &c2, NULL);

      r1[m]  = nn_add1(r1, a, m, c1);
      r1[m] += nn_add1(r1, r1, m, c2);
     
      r2[m]  = nn_add1(r2, a, m, c2);
      r2[m] += nn_add1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx, c2 = %wx\n", m, c1, c2);
         print_debug(a, m);  
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of add1 */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_add1(r1, a, m, c1);
      r1[m + n] = nn_add1(r1 + m, a + m, n, ci);
      
      r2[m + n] = nn_add1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, c1 = %wx\n", m, n, c1);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_sub1(void)
{
   int result = 1;
   nn_t a, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_sub1...");

   /* test a - c1 - c2 = a - c2 - c1 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, &c2, NULL);

      r1[m] = -nn_sub1(r1, a, m, c1);
      r1[m] -= nn_sub1(r1, r1, m, c2);
      
      r2[m] = -nn_sub1(r2, a, m, c2);
      r2[m] -= nn_sub1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx, c2 = %wx\n", m, c1, c2);
         print_debug(a, m);  
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of sub1 */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_sub1(r1, a, m, c1);
      r1[m + n] = -nn_sub1(r1 + m, a + m, n, ci);
      
      r2[m + n] = -nn_sub1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m, c1 = %wx\n", m, n, c1);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   /* test a + c1 - c1 = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m + 1, ANY, state, &a, &r1, NULL);
      a[m] = 0;

      randoms(ANY, state, &c1, NULL);
      
      r1[m]  = nn_add1(r1, a, m, c1);
      r1[m] -= nn_sub1(r1, r1, m, c1);
      
      result = nn_equal_m(r1, a, m + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, c1 = %wx\n", m, c1);
         print_debug(a, m + 1);  
         print_debug_diff(r1, a, m + 1);
      }
   } TEST_END;

   return result;
}

int test_not(void)
{
   int result = 1;
   nn_t a, r1;
   len_t m;

   printf("nn_not...");

   /* test not not a = a */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
      
      nn_not(r1, a, m);
      nn_not(r1, r1, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result) 
      {
         print_debug(a, m);  
         print_debug_diff(r1, a, m);
      }
   } TEST_END;

   return result;
}

int test_popcount(void)
{
   int result = 1;
   nn_t a;
   len_t m;
   bits_t i, count1, count2;

   printf("nn_popcount...");

   /* test popcount against naive nn_bit_test algorithm */
   TEST_START(1, ITER)
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_of_len(m, ANY, state, &a, NULL);

      count1 = nn_popcount(a, m);
      count2 = 0;
      for (i = 0; i < m * WORD_BITS; i++)
         count2 += !!nn_bit_test(a, i);

      result = (count1 == count2);
      if (!result)
         print_debug(a, m);
   } TEST_END;

   return result;
}

int test_scan1(void)
{
   int result = 1;
   nn_t a;
   len_t m;
   bits_t skip, b1, b2;

   printf("nn_scan1...");

   /* test scan1 against naive nn_bit_test algorithm */
   TEST_START(1, ITER)
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m * WORD_BITS, ANY, state, &skip, NULL);
      randoms_of_len(m, ANY, state, &a, NULL);

      b1 = nn_scan1(skip, a, m);
      for (b2 = skip; b2 < m * WORD_BITS; b2++)
      {
         if (nn_bit_test(a, b2))
            break;
      }
      if (b2 == m * WORD_BITS)
         b2 = -1;

      result = (b1 == b2);
      if (!result)
      {
         printf(BITS_FMT " " BITS_FMT "\n", b1, b2);
         print_debug(a, m);
      }
   } TEST_END;

   return result;
}

int test_hamdist_m(void)
{
   int result = 1;
   nn_t a, b, r1;
   len_t m;
   bits_t dist1, dist2;

   printf("nn_hamdist_m...");

   /* test hamdist against xor & popcount */
   TEST_START(1, ITER)
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_of_len(m, ANY, state, &a, &b, &r1, NULL);

      dist1 = nn_hamdist_m(a, b, m);
      nn_xor_m(r1, a, b, m);
      dist2 = nn_popcount(r1, m);

      result = (dist1 == dist2);
      if (!result)
      {
         print_debug(a, m);
         print_debug_diff(r1, a, m);
      }
   } TEST_END;

   return result;
}

int test_neg(void)
{
   int result = 1;
   nn_t a, r1, r2;
   len_t m, n;
   word_t ci;
   
   printf("nn_neg...");

   /* test neg a = (not a) + 1 */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
         
      r1[m] = -nn_neg(r1, a, m);

      nn_not(r2, a, m);
      r2[m]  = ~ (word_t) 0;
      r2[m] += nn_add1(r2, r2, m, 1);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) 
      {
         print_debug(a, m);  
         print_debug_diff(r1, r2, m + 1);
      }
   } TEST_END;

   /* test chaining of neg */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
         
      r1[m + n] = -nn_neg(r1, a, m + n);
      
      ci = nn_neg(r2, a, m);
      r2[m + n] = -nn_neg_c(r2 + m, a + m, n, ci);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) 
      {
         bsdnt_printf("m = %m, n = %m\n", m, n);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n + 1);
      }
   } TEST_END;

   return result;
}

int test_cmp_m(void)
{
   int result = 1;
   nn_t a, b, r1, r2;
   len_t m1;
   
   printf("nn_cmp_m...");

   /* test that equal things compare equal */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp_m(r1, r2, m1) == 0);

      if (!result) 
      {
         print_debug(r1, m1);  
         print_debug_diff(r1, r2, m1);
      }
   } TEST_END;

   /* test that not equal values compare in the correct way */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         r1[m1] = nn_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = (nn_cmp_m(r1, a, m1) > 0 && nn_cmp_m(a, r1, m1) < 0);

      if (!result) 
      {
         print_debug(a, m1); print_debug(b, m1); print_debug(r1, m1);  
         print_debug_diff(r1, a, m1);
      }
   } TEST_END;

   return result;
}

int test_cmp(void)
{
   int result = 1;
   nn_t a, b, r1, r2;
   len_t m1, m2;
   
   printf("nn_cmp...");

   /* test that equal things compare equal */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp(r1, m1, r2, m1) == 0);

      if (!result) 
      {
         print_debug(r1, m1);  
         print_debug_diff(r1, r2, m1);
      }
   } TEST_END;

   /* test that not equal lengths compare in the correct way */
   TEST_START(2, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, NULL);
      
      randoms_of_len(m1, FULL, state, &r1, NULL);
      randoms_of_len(m2, FULL, state, &r2, NULL);
      
      result = (nn_cmp(r1, m1, r2, m2) > 0 && nn_cmp(r2, m2, r1, m1) < 0);

      if (!result) 
      {
         print_debug(r1, m1); print_debug(r2, m1);  
      }
   } TEST_END;

   /* test that not equal values compare in the correct way */
   TEST_START(3, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         r1[m1] = nn_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = (nn_cmp(r1, m1, a, m1) > 0 && nn_cmp(a, m1, r1, m1) < 0);

      if (!result) 
      {
         print_debug(a, m1); print_debug(b, m1); print_debug(r1, m1);  
         print_debug_diff(r1, a, m1);
      }
   } TEST_END;

   return result;
}

int test_divrem1_simple(void)
{
   int result = 1;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, ci, r;

   printf("nn_divrem1_simple...");

   /* test that a = q * d + r */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &r1, &a, &q, NULL);
      
      randoms(NONZERO, state, &d, NULL);
      
      r = nn_divrem1_simple(q, a, m, d);

      ci = nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, d = %wx\n", ci, d);
         print_debug(a, m); print_debug(q, m); print_debug(r1, m);  
         print_debug_diff(r1, a, m);
      }
   } TEST_END;

   /* test chaining of divrem1_simple */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);
      
      randoms(NONZERO, state, &d, NULL);
      
        
      ci = nn_divrem1_simple(r1 + n, a + n, m, d);
      nn_divrem1_simple_c(r1, a, n, d, ci);
      
      nn_divrem1_simple(r2, a, m + n, d);

      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, d = %wx, m = %m, n = %m\n", ci, d, m, n);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_divrem1_preinv(void)
{
   int result = 1;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, ci, r, rem1, rem2;
   preinv1_t inv;

   printf("nn_divrem1_preinv...");

   /* test that a = q * d + r */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &r1, &a, &q, NULL);
      
      randoms(NORMALISED, state, &d, NULL);

      inv = precompute_inverse1(d);

      r = nn_divrem1_preinv(q, a, m, d, inv);
      
      ci = nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, r = %wx\n", ci, r);
         print_debug(a, m); print_debug(q, m);  
         print_debug_diff(r1, a, m);
      }
   } TEST_END;
   
   /* test chaining of divrem1_preinv */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);
      
      randoms(NORMALISED, state, &d, NULL);

      inv = precompute_inverse1(d);

      nn_random(a, state, m + n);
         
      ci = nn_divrem1_preinv(r1 + n, a + n, m, d, inv);
      rem1 = nn_divrem1_preinv_c(r1, a, n, d, inv, ci);

      rem2 = nn_divrem1_preinv(r2, a, m + n, d, inv);

      result = (nn_equal_m(r1, r2, m + n) && rem1 == rem2);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, rem1 = %wx, rem2 = %wx, m = %m, n = %m\n", 
            ci, rem1, rem2, m, n);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_divrem_hensel1_preinv(void)
{
   int result = 1;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, ci, r, rem1, rem2;
   hensel_preinv1_t inv;

   printf("nn_divrem_hensel1_preinv...");

   /* test that a + B^m * r = q * d */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &r1, &a, &q, NULL);

      randoms(ODD, state, &d, NULL);

      precompute_hensel_inverse1(&inv, d);

      r = nn_divrem_hensel1_preinv(q, a, m, d, inv);

      ci = nn_mul1(r1, q, m, d);

      result = (nn_equal_m(r1, a, m) && ci == r);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, r = %wx\n, inv = %wx", ci, r, inv);
         print_debug(a, m); print_debug(q, m);  
         print_debug_diff(r1, a, m);
      }
   } TEST_END;

   /* test chaining of divrem_hensel1_preinv */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);

      randoms(ODD, state, &d, NULL);

      precompute_hensel_inverse1(&inv, d);        
      ci = nn_divrem_hensel1_preinv(r1, a, m, d, inv);
      rem1 = nn_divrem_hensel1_preinv_c(r1 + m, a + m, n, d, inv, ci);
      
      rem2 = nn_divrem_hensel1_preinv(r2, a, m + n, d, inv);

      result = (nn_equal_m(r1, r2, m + n) && (rem1 == rem2));

      if (!result) 
      {
         bsdnt_printf("ci = %wx, rem1 = %wx, rem2 = %wx, m = %m, n = %m, d = %wx, inv = %wx\n", 
            ci, rem1, rem2, m, n, d, inv);
         print_debug(a, m + n);  
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_mod1_preinv(void)
{
   int result = 1;
   nn_t a, a2, q;
   len_t m, n;
   word_t d, ci, rem1, rem2;
   preinv1_t inv;
   mod_preinv1_t minv;
   bits_t norm;

   printf("nn_mod1_preinv...");

   /* test that divrem1 and mod1 return same remainder */
   TEST_START(1, ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &a2, &q, NULL);

      randoms(NONZERO, state, &d, NULL);

      norm = high_zero_bits(d);
      ci = nn_shl(a2, a, m, norm);

      inv = precompute_inverse1(d << norm);
      rem1 = nn_divrem1_preinv_c(q, a2, m, d << norm, inv, ci) >> norm;
      
      precompute_mod_inverse1(&minv, d);
      rem2 = nn_mod1_preinv(a, m, d, minv);  
      
      result = (rem1 == rem2);

      if (!result) 
      {
         bsdnt_printf("rem1 = %wx, rem2 = %wx, m = %m, d = %wx, "
            "minv.b1 = %wx, minv.b2 = %wx, minv.b3 = %wx\n", 
            rem1, rem2, m, d, minv.b1, minv.b2, minv.b3);
         print_debug(a, m); print_debug(q, m);  
      }
   } TEST_END;

   /* test chaining of mod1_preinv */
   TEST_START(chaining, ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, &q, NULL);

      randoms(NONZERO, state, &d, NULL);

      precompute_mod_inverse1(&minv, d);
      ci = nn_mod1_preinv(a + n, m, d, minv);
      rem1 = nn_mod1_preinv_c(a, n, d, minv, ci);
      
      rem2 = nn_mod1_preinv(a, m + n, d, minv);

      result = (rem1 == rem2);

      if (!result) 
      {
         bsdnt_printf("ci = %wx, rem1 = %wx, rem2 = %wx, m = %m, n = %m, d = %wx, "
            "minv.b1 = %wx, minv.b2 = %wx, minv.b3 = %wx\n", 
            ci, rem1, rem2, m, n, d, minv.b1, minv.b2, minv.b3);
         print_debug(a, m + n);  
      }
   } TEST_END;

   return result;
}

int test_linear(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_gc);
   RUN(test_tmp);
   RUN(test_bit_set);
   RUN(test_bit_clear);
   RUN(test_not);
   RUN(test_popcount);
   RUN(test_scan1);
   RUN(test_hamdist_m);
   RUN(test_neg);
   RUN(test_add1);
   RUN(test_add_m);
   RUN(test_add);
   RUN(test_sub1);
   RUN(test_sub_m);
   RUN(test_sub);
   RUN(test_shl);
   RUN(test_shr);
   RUN(test_copy);
   RUN(test_equal_m);
   RUN(test_equal);
   RUN(test_zero);
   RUN(test_normalise);
   RUN(test_mul1);
   RUN(test_addmul1);
   RUN(test_submul1);
   RUN(test_cmp_m);
   RUN(test_cmp);
   RUN(test_divrem1_simple);
   RUN(test_divrem1_preinv);
   RUN(test_divrem_hensel1_preinv);
   RUN(test_mod1_preinv);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting nn_linear functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_linear();
	
   randclear(state);

   return ret;
}
