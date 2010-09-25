/* 
  Copyright (C) 2010, William Hart

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
#include <crtdbg.h>
#include "nn.h"
#include "test.h"

#define ITER 50000

rand_t state;

int test_gc(void)
{
   int result = 1;
   len_t m;
   word_t w1, w2, w3, w4;
   nn_t a, b, c;

   printf("gc...");

   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &w1, &w2, &w3, &w4, NULL);
      
      randoms_of_len(w1, ANY, state, &a, &b, &c, NULL);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, &c, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      nn_s_add_m(r1, a, b, m);
      nn_s_add_m(r1, r1, c, m);

      nn_s_add_m(r2, b, c, m);
      nn_s_add_m(r2, r2, a, m);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   /* test chaining of addition */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);

      randoms_of_len(m + n, ANY, state, &a, &b, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
       
      ci = nn_add_m(r1, a, b, m);
      nn_s_add_mc(r1 + m, a + m, b + m, n, ci);

      nn_s_add_m(r2, a, b, m + n);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, &m3, NULL);
      
      randoms_of_len(m1, ANY, state, &a, NULL);
      randoms_of_len(m2, ANY, state, &b, NULL);
      randoms_of_len(m3, ANY, state, &c, NULL);    
      randoms_of_len(m1 + 1, ANY, state, &r1, &r2, NULL);
      
      nn_s_add(r1, a, m1, b, m2);
      nn_s_add(r1, r1, m1, c, m3);

      nn_s_add(r2, a, m1, c, m3);
      nn_s_add(r2, r2, m1, b, m2);

      result = nn_equal_m(r1, r2, m1 + 1);

      if (!result) printf("m1 = %ld, m2 = %ld, m3 = %ld\n", m1, m2, m3);
   } TEST_END;

   /* test chaining of addition */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m1, &m2, &m3, NULL);
      
      randoms_of_len(m1 + m2 + m3, ANY, state, &a, NULL);
      randoms_of_len(m2 + m3, ANY, state, &b, NULL);
      randoms_of_len(m1 + m2 + m3 + 1, ANY, state, &r1, &r2, NULL);
      
      ci = nn_add(r1, a, m3, b, m3);
      nn_s_add_c(r1 + m3, a + m3, m1 + m2, b + m3, m2, ci);

      nn_s_add(r2, a, m1 + m2 + m3, b, m2 + m3);
      
      result = nn_equal_m(r1, r2, m1 + m2 + m3 + 1);

      if (!result) printf("m1 = %ld, m2 = %ld, m3 = %ld\n", m1, m2, m3);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, &c, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      nn_s_sub_m(r1, a, b, m);
      nn_s_sub_m(r1, r1, c, m);

      nn_s_sub_m(r2, a, c, m);
      nn_s_sub_m(r2, r2, b, m);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   /* test chaining of subtraction */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);

      randoms_of_len(m + n, ANY, state, &a, &b, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
       
      ci = nn_sub_m(r1, a, b, m);
      nn_s_sub_mc(r1 + m, a + m, b + m, n, ci);

      nn_s_sub_m(r2, a, b, m + n);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, NULL);
      
      nn_s_add_m(r1, a, b, m);
      nn_s_sub_m(r1, r1, b, m);

      result = (nn_equal_m(r1, a, m) && (r1[m] == 0));

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);

      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      nn_s_neg(r1, b, m);
      nn_s_add_m(r1, r1, a, m);
     
      nn_s_sub_m(r2, a, b, m);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld\n", m);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, &m3, NULL);
      
      randoms_of_len(m1, ANY, state, &a, NULL);
      randoms_of_len(m2, ANY, state, &b, NULL);
      randoms_of_len(m3, ANY, state, &c, NULL);    
      randoms_of_len(m1 + 1, ANY, state, &r1, &r2, NULL);
      
      nn_s_sub(r1, a, m1, b, m2);
      nn_s_sub(r1, r1, m1, c, m3);

      nn_s_sub(r2, a, m1, c, m3);
      nn_s_sub(r2, r2, m1, b, m2);

      result = nn_equal_m(r1, r2, m1 + 1);

      if (!result) printf("m1 = %ld, m2 = %ld, m3 = %ld\n", m1, m2, m3);
   } TEST_END;

   /* test chaining of subtraction */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m1, &m2, &m3, NULL);
      
      randoms_of_len(m1 + m2 + m3, ANY, state, &a, NULL);
      randoms_of_len(m2 + m3, ANY, state, &b, NULL);
      randoms_of_len(m1 + m2 + m3 + 1, ANY, state, &r1, &r2, NULL);
      
      ci = nn_sub(r1, a, m3, b, m3);
      nn_s_sub_c(r1 + m3, a + m3, m1 + m2, b + m3, m2, ci);

      nn_s_sub(r2, a, m1 + m2 + m3, b, m2 + m3);
      
      result = nn_equal_m(r1, r2, m1 + m2 + m3 + 1);

      if (!result) printf("m1 = %ld, m2 = %ld, m3 = %ld\n", m1, m2, m3);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      randoms_upto(WORD_BITS - sh1, ANY, state, &sh2, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
               
      nn_s_shl(r1, a, m, sh1);
      nn_shl(r1, r1, m + 1, sh2);

      nn_s_shl(r2, a, m, sh2);
      nn_shl(r2, r2, m + 1, sh1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
   } TEST_END;

   /* test chaining of shl */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
         
      ci = nn_shl(r1, a, m, sh1);
      nn_s_shl_c(r1 + m, a + m, n, sh1, ci);
      
      nn_s_shl(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   /* test a << 1 = a + a */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
         
      nn_s_shl(r1, a, m, 1);
      
      nn_s_add_m(r2, a, a, m);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld\n", m);
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
   TEST_START(ITER) 
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

      if (!result) printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
   } TEST_END;

   /* test chaining of shr */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, &r1, &r2, NULL);

      ci = nn_shr(r1 + n, a + n, m, sh1);
      nn_shr_c(r1, a, n, sh1, ci);
      
      nn_shr(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   }  TEST_END;

   /* test (a << sh1) >> sh1 = a */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      randoms_upto(WORD_BITS, ANY, state, &sh1, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r2, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, NULL);
      
      nn_s_shl(r1, a, m, sh1);
      
      nn_r_shr(r2, r1, m, sh1);
      
      result = nn_equal_m(a, r2, m);

      if (!result) printf("m = %ld, sh1 = %ld\n", m, sh1);
   }  TEST_END;

   return result;
}

int test_copy(void)
{
   int result = 1;
   nn_t a, r1;
   len_t m;

   printf("nn_copy...");

   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
         
      nn_copy(r1, a, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   return result;
}

int test_equal_m(void)
{
   int result = 1;
   long s;
   nn_t a, r1;
   len_t m;

   printf("nn_equal_m...");

   /* test copying and then modifiying yields non-equal integer */
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &s, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
      
      nn_copy(r1, a, m);
      a[s] += 1;

      result = !nn_equal_m(r1, a, m);

      if (!result) printf("m = %ld\n", m);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = nn_equal(r1, m1, r2, m1);

      if (!result) printf("m1 = %ld\n", m1);
   } TEST_END;

   /* test that not equal lengths compare not equal */
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, NULL);
      
      randoms_of_len(m1, FULL, state, &r1, NULL);
      randoms_of_len(m2, FULL, state, &r2, NULL);
            
      result = !nn_equal(r1, m1, r2, m2);

      if (!result) printf("m1 = %ld, m2 = %ld\n", m1, m2);
   } TEST_END;

   /* test that not equal values compare not equal */
   TEST_START(ITER) 
   {  
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         nn_s_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = !nn_equal(r1, m1, a, m1);

	  if (!result) printf("m1 = %ld, m2 = %ld\n", m1, m2);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
         
      nn_zero(a, m);

      result = (nn_normalise(a, m) == 0);

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   return result;
}

int test_normalise(void)
{
   int result = 1;
   long s1, s2;
   nn_t a, r1;
   len_t m;

   printf("nn_normalise...");

   /* test normalising then copying normalised part yields same integer */
   TEST_START(ITER) 
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

      if (!result) printf("m = %ld, s1 = %ld, s2 = %ld\n", m, s1, s2);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &t1, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      nn_s_mul1(t1, a, m, c1);
      nn_s_mul1(r1, a, m, c2);
      nn_add_m(r1, r1, t1, m + 1);
      
      nn_s_mul1(r2, a, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
   } TEST_END;

   /* test chaining of mul1 */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_mul1(r1, a, m, c1);
      nn_s_mul1_c(r1 + m, a + m, n, c1, ci);
      
      nn_s_mul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %ld, c1 = %lu\n", m, n, c1);
   } TEST_END;

   return result;
}

int test_addmul1(void)
{
   int result = 1;
   nn_t a, b, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_addmul1...");

   /* test a + b * (c1 + c2) = a + b * c1 + b * c2 */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &t1, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      r1[m] = nn_addmul1(r1, b, m, c1);
      nn_s_addmul1(r1, b, m, c2);
      
      r2[m] = nn_addmul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, c1 = %ld, c2 = %ld\n", m, c1, c2);
   } TEST_END;

   /* test chaining of addmul1 */
   TEST_START(ITER) 
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

      if (!result) printf("m = %ld, n = %ld, c1 = %ld\n", m, n, c1);
   } TEST_END;

   return result;
}

int test_submul1(void)
{
   int result = 1;
   nn_t a, b, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_submul1...");

   /* test a - b * (c1 + c2) = a - b * c1 - b * c2 */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(m + 1, ANY, state, &t1, &r1, &r2, NULL);
      
      do randoms(ANY, state, &c1, &c2, NULL);
      while (c1 + c2 < c1);

      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      r1[m] = -nn_submul1(r1, b, m, c1);
      nn_s_submul1(r1, b, m, c2);
      
      r2[m] = -nn_submul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, c1 = %ld, c2 = %ld\n", m, c1, c2);
   } TEST_END;

   /* test chaining of submul1 */
   TEST_START(ITER) 
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

      if (!result) printf("m = %ld, n = %ld, c1 = %ld\n", m, n, c1);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, &c2, NULL);

      nn_s_add1(r1, a, m, c1);
      nn_s_add1(r1, r1, m, c2);
     
      nn_s_add1(r2, a, m, c2);
      nn_s_add1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
   } TEST_END;

   /* test chaining of add1 */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_add1(r1, a, m, c1);
      nn_s_add1(r1 + m, a + m, n, ci);
      
      nn_s_add1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %lu, c1 = %lu\n", m, n, c1);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, &c2, NULL);

      nn_s_sub1(r1, a, m, c1);
      nn_s_sub1(r1, r1, m, c2);
      
      nn_s_sub1(r2, a, m, c2);
      nn_s_sub1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
   } TEST_END;

   /* test chaining of sub1 */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
      
      randoms(ANY, state, &c1, NULL);

      ci = nn_sub1(r1, a, m, c1);
      nn_s_sub1(r1 + m, a + m, n, ci);
      
      nn_s_sub1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %lu, c1 = %lu\n", m, n, c1);
   } TEST_END;

   /* test a + c1 - c1 = a */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m + 1, ANY, state, &a, &r1, NULL);
      a[m] = 0;

      randoms(ANY, state, &c1, NULL);
      
      nn_s_add1(r1, a, m, c1);
      nn_s_sub1(r1, r1, m, c1);
      
      result = nn_equal_m(r1, a, m + 1);

      if (!result) printf("m = %ld, c1 = %lu\n", m, c1);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &r1, NULL);
      
      nn_not(r1, a, m);
      nn_not(r1, r1, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result) printf("m = %ld\n", m);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(m + 1, ANY, state, &r1, &r2, NULL);
         
      nn_s_neg(r1, a, m);

      nn_not(r2, a, m);
      r2[m] = ~ (word_t) 0;
      nn_s_add1(r2, r2, m, 1);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result) printf("m = %ld\n", m);
   } TEST_END;

   /* test chaining of neg */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, NULL);
      randoms_of_len(m + n + 1, ANY, state, &r1, &r2, NULL);
         
      nn_s_neg(r1, a, m + n);
      
      ci = nn_neg(r2, a, m);
      nn_s_neg_c(r2 + m, a + m, n, ci);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp_m(r1, r2, m1) == 0);

      if (!result) printf("m1 = %ld\n", m1);
   } TEST_END;

   /* test that not equal values compare in the correct way */
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         nn_s_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = (nn_cmp_m(r1, a, m1) > 0 && nn_cmp_m(a, r1, m1) < 0);

      if (!result) printf("m1 = %ld\n", m1);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m1, NULL);
      
      randoms_of_len(m1, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp(r1, m1, r2, m1) == 0);

      if (!result) printf("m1 = %ld\n", m1);
   } TEST_END;

   /* test that not equal lengths compare in the correct way */
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      randoms_upto(m1, ANY, state, &m2, NULL);
      
      randoms_of_len(m1, FULL, state, &r1, NULL);
      randoms_of_len(m2, FULL, state, &r2, NULL);
      
      result = (nn_cmp(r1, m1, r2, m2) > 0 && nn_cmp(r2, m2, r1, m1) < 0);

      if (!result) printf("m1 = %ld, m2 = %ld\n", m1, m2);
   } TEST_END;

   /* test that not equal values compare in the correct way */
   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m1, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, &r1, NULL);
      
      do {
         randoms_of_len(m1, FULL, state, &a, &b, NULL);
         nn_s_add_m(r1, a, b, m1);
      } while (r1[m1]);
      
      result = (nn_cmp(r1, m1, a, m1) > 0 && nn_cmp(a, m1, r1, m1) < 0);

      if (!result) printf("m1 = %ld, m2 = %ld\n", m1, m2);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m + 1, ANY, state, &r1, &a, &q, NULL);
      
      randoms(NONZERO, state, &d, NULL);
      
      r = nn_divrem1_simple(q, a, m, d);

      ci = nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result) printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
   } TEST_END;

   /* test chaining of divrem1_simple */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);
      
      randoms(NONZERO, state, &d, NULL);
      
        
      ci = nn_divrem1_simple(r1 + n, a + n, m, d);
      nn_divrem1_simple_c(r1, a, n, d, ci);
      
      nn_divrem1_simple(r2, a, m + n, d);

      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld, d = %lu\n", m, n, d);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &r1, &a, &q, NULL);
      
      randoms(NONZERO, state, &d, NULL);

      precompute_inverse1(&inv, d);

      r = nn_divrem1_preinv(q, a, m, d, inv);
      
      ci = nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result) printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
   } TEST_END;

   /* test chaining of divrem1_preinv */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);
      
      randoms(NONZERO, state, &d, NULL);

      r1 = nn_init(m + n);
      r2 = nn_init(m + n);

      do {
         d = randword(state);
      } while (d == 0);

      precompute_inverse1(&inv, d);

      nn_random(a, state, m + n);
         
      ci = nn_divrem1_preinv(r1 + n, a + n, m, d, inv);
      rem1 = nn_divrem1_preinv_c(r1, a, n, d, inv, ci);

      rem2 = nn_divrem1_preinv(r2, a, m + n, d, inv);

      result = (nn_equal_m(r1, r2, m + n) && rem1 == rem2);

      if (!result) printf("m = %ld, n = %ld, d = %lu, rem1 = %lu, rem2 = %lu\n", 
            m, n, d, rem1, rem2);
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
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &r1, &a, &q, NULL);

      randoms(ODD, state, &d, NULL);

      precompute_hensel_inverse1(&inv, d);

      r = nn_divrem_hensel1_preinv(q, a, m, d, inv);

      ci = nn_mul1(r1, q, m, d);

      result = (nn_equal_m(r1, a, m) && ci == r);

      if (!result) printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
   } TEST_END;

   /* test chaining of divrem_hensel1_preinv */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &r1, &r2, &a, NULL);

      randoms(ODD, state, &d, NULL);

      precompute_hensel_inverse1(&inv, d);        
      ci = nn_divrem_hensel1_preinv(r1, a, m, d, inv);
      rem1 = nn_divrem_hensel1_preinv_c(r1 + m, a + m, n, d, inv, ci);
      
      rem2 = nn_divrem_hensel1_preinv(r2, a, m + n, d, inv);

      result = (nn_equal_m(r1, r2, m + n) && (rem1 == rem2));

      if (!result) printf("m = %ld, n = %ld, d = %lu, rem1 = %lu, rem2 = %lu\n", 

            m, n, d, rem1, rem2);
   } TEST_END;

   return result;
}

int test_mod1_preinv(void)
{
   int result = 1;
   nn_t a, q;
   len_t m, n;
   word_t d, ci, rem1, rem2;
   preinv1_t inv;
   mod_preinv1_t minv;

   printf("nn_mod1_preinv...");

   /* test that divrem1 and mod1 return same remainder */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q, NULL);

      randoms(NONZERO, state, &d, NULL);

      precompute_inverse1(&inv, d);
      rem1 = nn_divrem1_preinv(q, a, m, d, inv);
      
      precompute_mod_inverse1(&minv, d);
      rem2 = nn_mod1_preinv(a, m, d, minv);  
      
      result = (rem1 == rem2);

      if (!result) printf("m = %ld, rem1 = %lu, rem2 = %lu\n", m, rem1, rem2);
   } TEST_END;

   /* test chaining of mod1_preinv */
   TEST_START(ITER) 
   {
      randoms_upto(100, ANY, state, &m, &n, NULL);
      
      randoms_of_len(m + n, ANY, state, &a, &q, NULL);

      randoms(NONZERO, state, &d, NULL);

      precompute_mod_inverse1(&minv, d);
      ci = nn_mod1_preinv(a + n, m, d, minv);
      rem1 = nn_mod1_preinv_c(a, n, d, minv, ci);
      
      rem2 = nn_mod1_preinv(a, m + n, d, minv);

      result = (rem1 == rem2);

      if (!result) printf("m = %ld, n = %ld, d = %ld, rem1 = %lu, rem2 = %lu\n", 
            m, n, d, rem1, rem2);
   } TEST_END;

   return result;
}

int main(void)
{
   long pass = 0;
   long fail = 0;
   
   randinit(state);
   RUN(test_gc);
   RUN(test_not);
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

   randclear(state);

   return 0;
}

