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

int test_seti_equali(void)
{
   int result = 1;
   zz_t r1;
   sword_t c1, c2;

   printf("zz_seti/equali...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);

      zz_seti(r1, c1);
      
      result = zz_equali(r1, c1);

      if (!result) 
      {
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a != a + b if b != 0 */
   TEST_START(2, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      do {
         c2 = (sword_t) randword(state);
      } while (c2 == 0);

      zz_seti(r1, (sword_t) ((word_t) c1 + (word_t) c2));
      
      result = !zz_equali(r1, c1);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_set_equal(void)
{
   int result = 1;
   zz_t r1, a, b;
   len_t m1, m2;

   printf("zz_set/equal...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_set(r1, a);
      
      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a != a + b if b != 0 */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
     
      zz_add(r1, a, b);
      
      result = !zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_neg(void)
{
   int result = 1;
   zz_t r1, a;
   len_t m1;

   printf("zz_neg...");

   /* test a = -(-a) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_neg(r1, a);
      zz_neg(r1, r1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_swap(void)
{
   int result = 1;
   zz_t r1, r2, a, b;
   len_t m1, m2;

   printf("zz_swap...");

   /* test swap */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_set(r1, a);
      zz_set(r2, b);
      zz_swap(a, b);

      result = (zz_equal(r1, b) && zz_equal(r2, a));

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_is_zero(void)
{
   int result = 1;
   zz_t r1;
   
   printf("zz_zero/is_zero...");

   /* test 0 = 0 */
   TEST_START(1, ITER) 
   {
      randoms_signed(10, ANY, state, &r1, NULL);
      
      zz_zero(r1);

      result = zz_is_zero(r1);

      if (!result) 
      {
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmpi(void)
{
   int result = 1;
   zz_t a, r1;
   sword_t c1;
   len_t m1;

   printf("zz_cmpi...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);

      zz_seti(r1, c1);
      
      result = (zz_cmpi(r1, c1) == 0);

      if (!result) 
      {
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a > b if a is positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_seti(r1, c1);
      zz_add(r1, r1, a);

      result = (zz_cmpi(r1, c1) > 0);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1);
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a < b if a is negative */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_seti(r1, c1);
      zz_neg(a, a);
      zz_add(r1, r1, a);

      result = (zz_cmpi(r1, c1) < 0);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1);
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmp(void)
{
   int result = 1;
   zz_t a, r1, r2;
   len_t m1, m2;

   printf("zz_cmp...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      
      zz_set(r1, a);
      
      result = (zz_cmp(r1, a) == 0);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a > b if a is positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, ANY, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);

      result = (zz_cmp(r2, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a < b if a is negative */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, ANY, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_neg(a, a);
      zz_add(r2, r1, a);

      result = (zz_cmp(r2, r1) < 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmpabs(void)
{
   int result = 1;
   zz_t a, r1, r2;
   len_t m1, m2;

   printf("zz_cmpabs...");

   /* test |a| = |+/- a| */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      
      zz_set(r1, a);
      if (randint(2, state) == 0)
         zz_neg(a, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);

      result = (zz_cmpabs(r1, a) == 0);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- (b + a)| > |+/- b| if a and b are positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, POSITIVE, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);
      if (randint(2, state) == 0)
         zz_neg(r2, r2);

      result = (zz_cmpabs(r2, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- b| < |+/- (b + a)| if a and b are positive */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, POSITIVE, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);
      if (randint(2, state) == 0)
         zz_neg(r2, r2);

      result = (zz_cmpabs(r1, r2) < 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- a| > 0 */
   TEST_START(4, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      if (randint(2, state) == 0)
         zz_neg(a, a);

      result = (zz_cmpabs(a, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_addi(void)
{
   int result = 1;
   zz_t a, b, r1, r2;
   len_t m1;
   sword_t c1, c2;
   
   printf("zz_addi...");

   /* test (a + b) + c = a + (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_addi(r1, a, c1);
      zz_addi(r1, r1, c2);

      zz_seti(r2, c1);
      zz_addi(r2, r2, c2);
      zz_add(r2, r2, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_addi, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_subi(void)
{
   int result = 1;
   zz_t a, b, r1, r2;
   len_t m1;
   sword_t c1, c2;
   
   printf("zz_subi...");

   /* test (a - b) - c = a - (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_subi(r1, a, c1);
      zz_subi(r1, r1, c2);

      zz_seti(r2, c1);
      zz_addi(r2, r2, c2);
      zz_sub(r2, a, r2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_addi(r1, a, c1);
      zz_subi(r1, r1, c1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      
      zz_subi(r1, a, c1);
      zz_addi(r2, a, -c1);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_subi, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_muli(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t1, t2;
   len_t m1;
   sword_t c1, c2;

   printf("zz_muli...");

   /* test a(b + c) = ab + ac */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &t1, &t2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_seti(t1, c1);
      zz_addi(t1, t1, c2);
      zz_mul(r1, a, t1);

      zz_muli(t1, a, c1);
      zz_muli(t2, a, c2);
      zz_add(r2, t1, t2);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_muli, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_mul_2exp(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t;
   len_t m1;
   bits_t c1, c2;

   printf("zz_mul_2exp...");

   /* test (a*2^b)*2^c = a*2^(b+c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, &r2, NULL);
      c1 = (bits_t) randint(200, state);
      c2 = (bits_t) randint(200, state);

      zz_mul_2exp(t, a, c1);
      zz_mul_2exp(r1, t, c2);

      zz_mul_2exp(r2, a, c1 + c2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (bits_t) randint(200, state);
      
      test_zz_aliasing_12i(zz_mul_2exp, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_div_2exp(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t;
   len_t m1;
   bits_t c1, c2;

   printf("zz_div_2exp...");

   /* test (a/2^b)/2^c = a/2^(b+c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, &r2, NULL);
      c1 = (bits_t) randint(200, state);
      c2 = (bits_t) randint(200, state);

      zz_div_2exp(t, a, c1);
      zz_div_2exp(r1, t, c2);

      zz_div_2exp(r2, a, c1 + c2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test (a*2^b)/2^b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, NULL);
      c1 = (bits_t) randint(200, state);
      
      zz_mul_2exp(t, a, c1);
      zz_div_2exp(r1, t, c1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w\n", c1); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (bits_t) randint(200, state);
      
      test_zz_aliasing_12i(zz_div_2exp, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_divremi(void)
{
   int result = 1;
   zz_t a, b, q, t1, t2;
   len_t m1;
   sword_t c1, r;

   printf("zz_divremi...");

   /* test a = bq + r, |r| < |b| */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &q, &t1, &t2, NULL);
      c1 = (sword_t) randword(state);
      if (c1 == 0) c1 = 1;
       
      r = zz_divremi(q, a, c1);

      zz_muli(t1, q, c1);
      zz_addi(t1, t1, r);

      result = (zz_equal(a, t1) && BSDNT_ABS(r) < BSDNT_ABS(c1));

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(q); 
         bsdnt_printf("%w, %w\n", c1, r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a*b/b == b */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &q, &t1, &t2, NULL);
      c1 = (sword_t) randword(state);
      if (c1 == 0) c1 = 1;
       
      zz_muli(t1, a, c1);
      r = zz_divremi(q, t1, c1);

      result = (zz_equal(a, q) && r == 0);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(q); 
         bsdnt_printf("%w, %w\n", c1, r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &a, NULL);
      randoms_signed(m1, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      if (c1 == 0) c1 = 1;

      test_zz_aliasing_12i(zz_divremi, a, b, c1);

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

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_add, a, b, c);

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

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_sub, a, b, c);

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

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_mul, a, b, c);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_divrem(void)
{
   int result = 1;
   zz_t a, b, q, r, t1;
   len_t m1, m2;
   sword_t c1, r2;

   printf("zz_divrem...");

   /* test a = bq + r, |r| < |b| */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
      randoms_signed(0, ANY, state, &q, &r, &t1, NULL);
       
      zz_divrem(q, r, a, b);

      zz_mul(t1, q, b);
      zz_add(t1, t1, r);

      result = (zz_equal(a, t1) && zz_cmpabs(r, b) < 0);
      /* remainder is zero or sign is same as that of b */
      result &= (zz_is_zero(r) || ((zz_cmpi(r, 0) ^ zz_cmpi(b, 0)) >= 0));

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(q); zz_print_debug(r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a*b/b == b */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
      randoms_signed(0, ANY, state, &q, &r, &t1, NULL);
       
      zz_mul(t1, a, b);
      zz_divrem(q, r, t1, b);

      result = (zz_equal(a, q) && zz_is_zero(r));
      
      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(q); zz_print_debug(r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;

   /* compare with divremi */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &q, &r, &t1, NULL);
      c1 = (sword_t) randword(state);
      if (c1 == 0) c1 = 1;

      zz_seti(b, c1);
      zz_divrem(q, r, a, b);
      r2 = zz_divremi(t1, a, c1);

      result = (zz_equal(q, t1) && zz_equali(r, r2));

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(q); zz_print_debug(r); 
         zz_print_debug(t1);
         bsdnt_printf("%w\n", r2);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(0, ANY, state, &q, &r, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
     
      test_zz_aliasing_22(zz_divrem, q, r, a, b);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_div(void)
{
   int result = 1;
   zz_t a, b, q, r, t1;
   len_t m1, m2;
   
   printf("zz_div...");

   /* compare with divrem */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
      randoms_signed(0, ANY, state, &q, &r, &t1, NULL);
       
      zz_divrem(q, r, a, b);
      zz_div(t1, a, b);

      result = zz_equal(q, t1);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(q); zz_print_debug(r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a*b/b == b */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
      randoms_signed(0, ANY, state, &q, &t1, NULL);
       
      zz_mul(t1, a, b);
      zz_div(q, t1, b);

      result = zz_equal(a, q);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(q); zz_print_debug(r); 
         zz_print_debug(t1);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(0, ANY, state, &q, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
     
      test_zz_aliasing_12(zz_div, q, a, b);

      gc_cleanup();
   } TEST_END;

   return result;
}

void _test_powi_print_helper(zz_t r)
{
   if (r->size > 100)
      printf(WORD_FMT "x..."WORD_FMT "x (" LEN_FMT ")\n", r->n[r->size - 1], r->n[0], r->size);
   else
      zz_print_debug(r);
}

int test_powi(void)
{
   int result = 1, explicit_test;
   zz_t a, r, r2, r3;
   len_t m;
   sword_t power, px, py, i;

   printf("zz_powi...");

   /* compare with trivial algorithm */
   TEST_START(1, ITER)
   {
      randoms_upto(10, ANY, state, &m, NULL);
      randoms_signed(m, ANY, state, &a, &r, &r2, NULL);

      randoms_upto(50, NONZERO, state, &power, NULL);

      zz_powi(r, a, power);

      zz_seti(r2, 1);
      for (i = 0; i < power; i++)
         zz_mul(r2, r2, a);

      /* Make sure that over-alloc isn't huge. */
      result = zz_equal(r, r2) && (r->alloc <= m || r->alloc/2 <= BSDNT_ABS(r->size));

      if (!result)
      {
         printf("size:" LEN_FMT " alloc:" LEN_FMT "\n", r->size, r->alloc);
         zz_print_debug(a);
         printf("^" WORD_FMT "d\n", power);
         zz_print_debug(r); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test small bases & larger powers */
   TEST_START(2, 5)
   {
      result = 1;
      randoms_signed(1, ANY, state, &a, &r, &r2, &r3, NULL);
      randoms_upto(99999, ANY, state, &power, NULL);

      for (i = 0; i < 100 && result; i++)
      {
         sword_t base = i & 1 ? -i/2 : i/2;
         zz_seti(a, base);
         zz_powi(r, a, power);

         /* explicit tests, only for particular bases. */
         explicit_test = 1;
         switch (BSDNT_ABS(base))
         {
            case 0:
               zz_seti(r2, 0);
               break;
            case 1:
               zz_seti(r2, 1);
               break;
            case 2: case 4: case 8: case 16: case 32: case 64:
               zz_seti(r2, 1);
               zz_mul_2exp(r2, r2, power * low_zero_bits(BSDNT_ABS(base)));
               break;
            default:
               explicit_test = 0;
               break;
         }

         if (explicit_test)
         {
            if (base < 0 && power % 2 == 1)
               zz_neg(r2, r2);

            if (!zz_equal(r, r2))
            {
               result = 0;
               printf(WORD_FMT "d^" WORD_FMT "d\n", base, power);
               _test_powi_print_helper(r);
               _test_powi_print_helper(r2);
            }
         }

         /* test base^x * base^y == base^(x+y) identity */
         randoms_upto(power, ANY, state, &px, NULL);
         py = power - px;
         zz_powi(r2, a, px);
         zz_powi(r3, a, py);
         zz_mul(r2, r2, r3);
         if (!zz_equal(r, r2))
         {
            printf("base = " WORD_FMT "d\n", base);
            printf("base^" WORD_FMT "d != base^" WORD_FMT "d * base^" WORD_FMT "d\n", power, px, py);
            _test_powi_print_helper(r);
            _test_powi_print_helper(r2);
         }
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER)
   {
      randoms_upto(10, ANY, state, &m, NULL);
      randoms_signed(m, ANY, state, &r, &a, NULL);

      randoms_upto(50, ANY, state, &power, NULL);

      test_zz_aliasing_12i(zz_powi, r, a, power);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_gcd(void)
{
   int result = 1;
   zz_t a, b, c, g;
   len_t m1, m2, m3;
   
   printf("zz_gcd...");

   /* test gcd(ac, bc) = c if gcd(a, b) = 1 */
   TEST_START(1, ITER/5) 
   {
      do { /* find coprime a and b */
         gc_cleanup();
         randoms_upto(10, ANY, state, &m1, &m2, NULL);
         randoms_signed(0, ANY, state, &g, NULL);
         randoms_signed(m1, ANY, state, &a, NULL);
         randoms_signed(m2, ANY, state, &b, NULL);
         zz_gcd(g, a, b);
      } while (!zz_equali(g, 1));
      
      randoms_upto(10, ANY, state, &m3, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      
      zz_mul(a, a, c);
      zz_mul(b, b, c);

      zz_gcd(g, a, b);

      /* gcd cannot be negative if a and b have different signs */
      if ((a->size ^ b->size) < 0 && a->size != 0 && b->size != 0)
         c->size = BSDNT_ABS(c->size);

      result = zz_equal(g, c);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(c); zz_print_debug(g); 
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER/5) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, ANY, state, &m2, NULL);

      randoms_signed(0, ANY, state, &g, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
     
      test_zz_aliasing_12(zz_gcd, g, a, b);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_xgcd(void)
{
   int result = 1;
   zz_t a, b, g, t1, t2, u, v;
   len_t m1, m2;
   
   printf("zz_xgcd...");

   /* compare with gcd */
   TEST_START(1, ITER/5) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);
         
      randoms_signed(0, ANY, state, &t1, &g, &u, &v, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
         
      zz_gcd(g, a, b);
      zz_xgcd(t1, u, v, a, b);

      result = zz_equal(g, t1);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(g); zz_print_debug(t1); 
      }

      gc_cleanup();
   } TEST_END;
   
   /* check g = au + bv */
   TEST_START(2, ITER/5) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);
         
      randoms_signed(0, ANY, state, &t1, &t2, &g, &u, &v, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
         
      zz_xgcd(g, u, v, a, b);

      zz_mul(t1, a, u);
      zz_mul(t2, b, v);
      zz_add(t1, t1, t2);

      result = zz_equal(g, t1);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(u); zz_print_debug(v); 
         zz_print_debug(g); zz_print_debug(t1); 
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER/5) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, ANY, state, &m2, NULL);

      randoms_signed(0, ANY, state, &g, NULL);
      randoms_signed(0, ANY, state, &u, NULL);
      randoms_signed(0, ANY, state, &v, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
     
      test_zz_aliasing_32(zz_xgcd, g, u, v, a, b);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_get_set_str(void)
{
   int result = 1;
   len_t m;
   zz_t a, b;
   char * str;
   size_t digits;

   printf("zz_get/set_str...");

   TEST_START(1, ITER/2) 
   {
      randoms_upto(10, NONZERO, state, &m, NULL);
      
      randoms_signed(m, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &b, NULL);

      str = zz_get_str(a);
      digits = zz_set_str(b, str);

      result = (zz_equal(a, b) && digits == strlen(str));
      
      if (!result)
      {
         zz_print_debug(a); zz_print_debug(b); 
         printf("str = %s\n", str);
         printf("digits = %ld\n", (long) digits);
      }

      free(str);
      gc_cleanup();
   } TEST_END;

   return result;
}


int test_zz(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_gc);
   RUN(test_seti_equali);
   RUN(test_set_equal);
   RUN(test_neg);
   RUN(test_swap);
   RUN(test_is_zero);
   RUN(test_cmpi);
   RUN(test_cmp);
   RUN(test_cmpabs);
   RUN(test_addi);
   RUN(test_subi);
   RUN(test_muli);
   RUN(test_mul_2exp);
   RUN(test_div_2exp);
   RUN(test_divremi);
   RUN(test_add);
   RUN(test_sub);
   RUN(test_mul);
   RUN(test_divrem);
   RUN(test_div);
   RUN(test_powi);
   RUN(test_gcd);
   RUN(test_xgcd);
   RUN(test_get_set_str);
   
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
