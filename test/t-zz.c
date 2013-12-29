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
#include "zz0.h"
#include "zz.h"
#include "test.h"

rand_t state;

#undef ITER
#define ITER 50000

int test_gc(void)
{
   int result = 1;
   len_t m;
   word_t w1, w2, w3, w4;
   zz_t a, b, c;

   printf("gc...");

   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &w1, &w2, &w3, &w4, NULL);
      
      randoms_signed(w1, ANY, state, &a, &b, &c, NULL);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_add(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   
   printf("zz_add...");

   /* test (a + b) + c = a + (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_add(r1, a, b);
      zz_add(r1, r1, c);

      zz_add(r2, b, c);
      zz_add(r2, r2, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);
         zz_print_debug(c);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_sub(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   
   printf("zz_sub...");

   /* test (a - b) - c = (a - c) - b */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_sub(r1, a, b);
      zz_sub(r1, r1, c);

      zz_sub(r2, a, c);
      zz_sub(r2, r2, b);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_add(r1, a, b);
      zz_sub(r1, r1, b);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &c, NULL);
      
      zz_sub(r1, a, b);
      zz_neg(c, b);
      zz_add(r2, a, c);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);  zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_mul(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2, t1, t2;
   len_t m1, m2, m3;
   
   printf("zz_mul...");

   /* test ab = ba */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_mul(r1, a, b);
      zz_mul(r2, b, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test a(b + c) = ac + bc */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &t1, &t2, NULL);
      
      zz_add(t1, b, c);
      zz_mul(r1, a, t1);

      zz_mul(t1, a, b);
      zz_mul(t2, a, c);
      zz_add(r2, t1, t2);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_zz(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_gc);
   RUN(test_add);
   RUN(test_sub);
   RUN(test_mul);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting zz functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_zz();
	
   randclear(state);

   return ret;
}
