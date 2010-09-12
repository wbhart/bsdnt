#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "generic.h"

#define ITER 100000

rand_t state;

#define TEST_START(XXX) \
   do { \
      long __count; \
      for (__count = 0; __count < XXX  && result == 1; __count++)

#define TEST_END \
      gc_cleanup(); \
   } while (0)

#define ASSERT_EQUAL(obj1, obj2, msg) \
   if (!equal(obj1, obj2)) \
   { \
      talker(msg); \
      result = 0; \
   }

/*
   Given a fn which can be chained with itself from right 
   to left, computes r = fn(a, b) and s = fn(a, b) where 
   r is computed by breaking a, b, r into chains with 
   segments of length m1, n for a and r, and m2, n for b.
*/
#define chain2_rl_1d_2s_test(fn, r, s, a, b, m1, m2, n) \
   do { \
      new_objs(NN, &a, &b, &r, &s, NULL); \
      randoms_of_len(m1 + n + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m1 + n, ANY, state, a, NULL); \
      randoms_of_len(m2 + n, ANY, state, b, NULL); \
      chain2_rl_1d_2s(fn, r, a, b, m1, m2, n); \
      fn(s, a, b, NULL); \
   } while (0)
 
/*
   Given a fn which can be chained with itself from left 
   to right, computes r = fn(a, b) and s = fn(a, b) where 
   r is computed by breaking a, b, r into chains with 
   segments of length m1, n for a and r, and m2, n for b.
*/
#define chain2_lr_1d_2s_test(fn, r, s, a, b, m1, m2, n) \
   do { \
      new_objs(NN, &a, &b, &r, &s, NULL); \
      randoms_of_len(m1 + n + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m1 + n, ANY, state, a, NULL); \
      randoms_of_len(m2 + n, ANY, state, b, NULL); \
      chain2_lr_1d_2s(fn, r, a, b, m1, m2, n); \
      fn(s, a, b, NULL); \
   } while (0)
 
/*
   Given a fn which can be chained with itself from right 
   to left, computes r = fn(a, c) and s = fn(a, c) where 
   r is computed by breaking a, r into chains with 
   segments of length m, n for a and r.
*/
#define chain2_rl_1d_1s_c_test(fn, r, s, a, c, m, n) \
   do { \
      new_objs(NN, &a, &r, &s, NULL); \
      randoms_of_len(m + n + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m + n, ANY, state, a, NULL); \
      chain2_rl_1d_1s_c(fn, r, a, c, m, n); \
      fn(s, a, c, NULL); \
   } while (0)
 
/*
   Given a fn which can be chained with itself from right 
   to left, computes r = fn(a, c) and s = fn(a, c) where 
   r is computed by breaking a, b, r into chains with 
   segments of length length m, n, p for a and r.
*/
#define chain3_rl_1d_1s_c_test(fn, r, s, a, c, m, n, p) \
   do { \
      new_objs(NN, &a, &r, &s, NULL); \
      randoms_of_len(m + n + p + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m + n + p, ANY, state, a, NULL); \
      chain3_rl_1d_1s_c(fn, r, a, c, m, n, p); \
      fn(s, a, c, NULL); \
   } while (0)

/*
   Given a fn which can be chained with itself from right 
   to left, computes r = fn(a, b) and s = fn(a, b) where 
   r is computed by breaking a, b, r into chains with 
   segments of length length m1, n, p for a and r, and m2, 
   n, p for b.
*/
#define chain3_rl_1d_2s_test(fn, r, s, a, b, m1, m2, n, p) \
   do { \
      new_objs(NN, &a, &b, &r, &s, NULL); \
      randoms_of_len(m1 + n + p + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m1 + n + p, ANY, state, a, NULL); \
      randoms_of_len(m2 + n + p, ANY, state, b, NULL); \
      chain3_rl_1d_2s(fn, r, a, b, m1, m2, n, p); \
      fn(s, a, b, NULL); \
   } while (0)

/*
   Given a fn which can be chained with itself from left 
   to right, computes r = fn(a, b) and s = fn(a, b) where 
   r is computed by breaking a, b, r into chains with 
   segments of length m1, n, p for a and r, and m2, n, p 
   for b.
*/
#define chain3_lr_1d_2s_test(fn, r, s, a, b, m1, m2, n, p) \
   do { \
      new_objs(NN, &a, &b, &r, &s, NULL); \
      randoms_of_len(m1 + n + p + 1, ANY, state, r, s, NULL); \
      randoms_of_len(m1 + n + p, ANY, state, a, NULL); \
      randoms_of_len(m2 + n + p, ANY, state, b, NULL); \
      chain3_lr_1d_2s(fn, r, a, b, m1, m2, n, p); \
      fn(s, a, b, NULL); \
   } while (0)

int test_add_m(void)
{
   int result = 1;
   len_t m, n, p;
   obj_t * a, * b, * c, * r, * s;
   
   printf("nn_add_m...");

   TEST_START(ITER) /* test (a + b) + c == (a + c) + b */
   {
      m = randint(100, state);
      
      new_objs(NN, &a, &b, &c, &r, &s, NULL);
      
      randoms_of_len(m + 1, ANY, state, r, s, NULL);
      randoms_of_len(m, ANY, state, a, b, c, NULL);

      add_m(r, a, b, NULL);
      add_m(r, r, c, NULL);

      add_m(s, a, c, NULL);
      add_m(s, s, b, NULL);

      ASSERT_EQUAL(r, s, "(a + b) + c != (a + c) + b");
   } TEST_END;

   TEST_START(ITER) /* test chaining of two add_m's */
   {   
      m = randint(100, state);
      n = randint(100, state);
      chain2_rl_1d_2s_test(add_m, r, s, a, b, m, m, n);
      
      ASSERT_EQUAL(r, s, "add_m fails chain2");
   } TEST_END;

   TEST_START(ITER) /* test chaining of three add_m's */
   {      
      m = randint(100, state);
      n = randint(100, state);
      p = randint(100, state);
      chain3_rl_1d_2s_test(add_m, r, s, a, b, m, m, n, p);

      ASSERT_EQUAL(r, s, "add_m fails chain3");
   } TEST_END;

   return result;
}

int test_add(void)
{
   int result = 1;
   len_t m1, m2, m3, n, p;
   obj_t * a, * b, * c, * r, * s;
   
   printf("nn_add...");

   TEST_START(ITER) /* test (a + b) + c == (a + c) + b */
   {
      m1 = randint(100, state) + 1;
      m2 = randint(m1, state);
      m3 = randint(m1, state);
      
      new_objs(NN, &a, &b, &c, &r, &s, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, r, s, NULL);
      randoms_of_len(m1, ANY, state, a, NULL);
      randoms_of_len(m2, ANY, state, b, NULL);
      randoms_of_len(m3, ANY, state, c, NULL);

      add(r, a, b, NULL);
      add(r, r, c, NULL);

      add(s, a, c, NULL);
      add(s, s, b, NULL);

      ASSERT_EQUAL(r, s, "(a + b) + c != (a + c) + b");
   } TEST_END;

   TEST_START(ITER) /* test chaining of two add's */
   {   
      m1 = randint(100, state);
      m2 = randint(m1 + 1, state);
      n = randint(100, state);
      chain2_rl_1d_2s_test(add, r, s, a, b, m1, m2, n);
      
      ASSERT_EQUAL(r, s, "add fails chain2");
   } TEST_END;

   TEST_START(ITER) /* test chaining of three add's */
   {      
      m1 = randint(100, state);
      m2 = randint(m1 + 1, state);
      n = randint(100, state);
      p = randint(100, state);
      chain3_rl_1d_2s_test(add, r, s, a, b, m1, m2, n, p);

      ASSERT_EQUAL(r, s, "add fails chain3");
   } TEST_END;

   return result;
}

int test_sub_m(void)
{
   int result = 1;
   len_t m, n, p;
   obj_t * a, * b, * c, * r, * s;
   
   printf("nn_sub_m...");

   TEST_START(ITER) /* test (a - b) - c == (a - c) - b */
   {
      m = randint(100, state);
      
      new_objs(NN, &a, &b, &c, &r, &s, NULL);
      
      randoms_of_len(m + 1, ANY, state, r, s, NULL);
      randoms_of_len(m, ANY, state, a, b, c, NULL);

      sub_m(r, a, b, NULL);
      sub_m(r, r, c, NULL);

      sub_m(s, a, c, NULL);
      sub_m(s, s, b, NULL);

      ASSERT_EQUAL(r, s, "(a - b) - c != (a - c) - b");
   } TEST_END;

   TEST_START(ITER) /* test (a + b) - b == a */
   {
      m = randint(100, state);
      
      new_objs(NN, &a, &b, &r, NULL);
      
      randoms_of_len(m + 1, ANY, state, r, NULL);
      randoms_of_len(m, ANY, state, a, b, NULL);

      add_m(r, a, b, NULL);
      sub_m(r, r, b, NULL);

      ASSERT_EQUAL(r, a, "(a + b) - b != a");
   } TEST_END;

   TEST_START(ITER) /* test a + (-b) == a - b */
   {
      m = randint(100, state);
      
      new_objs(NN, &a, &b, &r, &s, NULL);
      
      randoms_of_len(m + 1, ANY, state, r, s, NULL);
      randoms_of_len(m, ANY, state, a, b, NULL);

      neg(r, b, NULL);
      add_m(r, r, a, NULL);

      sub_m(s, a, b, NULL);

      ASSERT_EQUAL(r, s, "a + (-b) != a - b");
   } TEST_END;

   TEST_START(ITER) /* test chaining of two sub_m's */
   {   
      m = randint(100, state);
      n = randint(100, state);
      chain2_rl_1d_2s_test(sub_m, r, s, a, b, m, m, n);
      
      ASSERT_EQUAL(r, s, "sub_m fails chain2");
   } TEST_END;

   TEST_START(ITER) /* test chaining of three sub_m's */
   {      
      m = randint(100, state);
      n = randint(100, state);
      p = randint(100, state);
      chain3_rl_1d_2s_test(sub_m, r, s, a, b, m, m, n, p);

      ASSERT_EQUAL(r, s, "sub_m fails chain3");
   } TEST_END;

   return 1;
}

int test_sub(void)
{
   int result = 1;
   len_t m1, m2, m3, n, p;
   obj_t * a, * b, * c, * r, * s;
   
   printf("nn_sub...");

   TEST_START(ITER) /* test (a - b) - c == (a - c) - b */
   {
      m1 = randint(100, state) + 1;
      m2 = randint(m1, state);
      m3 = randint(m1, state);
      
      new_objs(NN, &a, &b, &c, &r, &s, NULL);
      
      randoms_of_len(m1 + 1, ANY, state, r, s, NULL);
      randoms_of_len(m1, ANY, state, a, NULL);
      randoms_of_len(m2, ANY, state, b, NULL);
      randoms_of_len(m3, ANY, state, c, NULL);

      sub(r, a, b, NULL);
      sub(r, r, c, NULL);

      sub(s, a, c, NULL);
      sub(s, s, b, NULL);

      ASSERT_EQUAL(r, s, "(a - b) - c != (a - c) - b");
   } TEST_END;

   TEST_START(ITER) /* test chaining of two sub's */
   {   
      m1 = randint(100, state);
      m2 = randint(m1 + 1, state);
      n = randint(100, state);
      chain2_rl_1d_2s_test(sub, r, s, a, b, m1, m2, n);
      
      ASSERT_EQUAL(r, s, "sub fails chain2");
   } TEST_END;

   TEST_START(ITER) /* test chaining of three add's */
   {      
      m1 = randint(100, state);
      m2 = randint(m1 + 1, state);
      n = randint(100, state);
      p = randint(100, state);
      chain3_rl_1d_2s_test(sub, r, s, a, b, m1, m2, n, p);

      ASSERT_EQUAL(r, s, "sub fails chain3");
   } TEST_END;

   return result;
}

int test_shl(void)
{
   int result = 1;
   len_t m, n, p;
   obj_t * a, * r, * s, * right;
   bits_t sh1, sh2;
   
   printf("nn_shl...");

   TEST_START(ITER) /* test (a << sh1) << sh2 = (a << sh2) << sh1 */
   {
      m = randint(100, state);

      sh1 = randint(WORD_BITS, state);
      sh2 = randint(WORD_BITS - sh1, state);
      
      new_objs(NN, &a, &r, &s, NULL);
      right = new_ctl(R);

      randoms_of_len(m + 1, ANY, state, r, s, NULL);
      randoms_of_len(m, ANY, state, a, NULL);
      
      shl(r, a, sh1, NULL);
      shl(r, r, sh2, right);

      shl(s, a, sh2, NULL);
      shl(s, s, sh1, right);

      ASSERT_EQUAL(r, s, "(a << sh1) << sh2 != (a << sh2) << sh1");
   } TEST_END;

   TEST_START(ITER) /* test (a << 1) = a + a */
   {
      m = randint(100, state);

      new_objs(NN, &a, &r, &s, NULL);
      
      randoms_of_len(m + 1, ANY, state, r, s, NULL);
      randoms_of_len(m, ANY, state, a, NULL);
      
      shl(r, a, 1, NULL);
      
      add_m(s, a, a, NULL);

      ASSERT_EQUAL(r, s, "(a << 1) != a + a");
   } TEST_END;

   TEST_START(ITER) /* test chaining of two shl's */
   {   
      m = randint(100, state);
      n = randint(100, state);
      sh1 = randint(WORD_BITS, state);

      chain2_rl_1d_1s_c_test(shl, r, s, a, sh1, m, n);
      
      ASSERT_EQUAL(r, s, "shl fails chain2");
   } TEST_END;

   TEST_START(ITER) /* test chaining of three shl's */
   {      
      m = randint(100, state);
      n = randint(100, state);
      p = randint(100, state);
      sh1 = randint(WORD_BITS, state);

      chain3_rl_1d_1s_c_test(shl, r, s, a, sh1, m, n, p);

      ASSERT_EQUAL(r, s, "shl fails chain3");
   } TEST_END;

   return result;
}

/*int test_shl(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m, n;
   word_t ci;

   printf("nn_shl...");

   test (a << sh1) << sh2 = (a << sh2) << sh1 
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
      sh1 = randint(WORD_BITS, state);
      sh2 = randint(WORD_BITS - sh1, state);
         
      nn_shl(r1, a, m, sh1);
      _nn_shl(r1, r1, m + 1, sh2);

      nn_shl(r2, a, m, sh2);
      _nn_shl(r2, r2, m + 1, sh1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   test chaining of shl 
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      sh1 = randint(WORD_BITS, state);
      
      nn_random(a, state, m + n);
         
      ci = _nn_shl(r1, a, m, sh1);
      nn_shl_c(r1 + m, a + m, n, sh1, ci);
      
      nn_shl(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %ld\n", m, n);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   test a << 1 = a + a 
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
         
      nn_shl(r1, a, m, 1);
      
      nn_add_m(r2, a, a, m);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
} */

int test_shr(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m, n;
   word_t ci;

   printf("nn_shr...");

   /* test (a >> sh1) >> sh2 = (a >> sh2) >> sh1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m);
      r2 = nn_init(m);

      nn_random(a, state, m);
      sh1 = randint(WORD_BITS, state);
      sh2 = randint(WORD_BITS - sh1, state);
         
      _nn_shr(r1, a, m, sh1);
      _nn_shr(r1, r1, m, sh2);

      _nn_shr(r2, a, m, sh2);
      _nn_shr(r2, r2, m, sh1);

      result = nn_equal_m(r1, r2, m);

      if (!result)
      {
         printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of shr */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n);
      r2 = nn_init(m + n);

      sh1 = randint(WORD_BITS, state);
      
      nn_random(a, state, m + n);
         
      ci = _nn_shr(r1 + n, a + n, m, sh1);
      _nn_shr_c(r1, a, n, sh1, ci);
      
      _nn_shr(r2, a, m + n, sh1);

      result = nn_equal_m(r1, r2, m + n);

      if (!result)
      {
         printf("m = %ld, n = %ld\n", m, n);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test (a << sh1) >> sh1 = a */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m);

      nn_random(a, state, m);
      sh1 = randint(WORD_BITS, state);
        
      nn_shl(r1, a, m, sh1);
      
      nn_shr(r2, r1, m, sh1);
      
      result = nn_equal_m(a, r2, m);

      if (!result)
      {
         printf("m = %ld, sh1 = %ld\n", m, sh1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_copy(void)
{
   int result = 1;
   long i;
   nn_t a, r1;
   len_t m;

   printf("nn_copy...");

   /* test copying yields equal integer */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m);
      
      nn_random(a, state, m);
         
      nn_copy(r1, a, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
   }

   return result;
}

int test_equal_m(void)
{
   int result = 1;
   long i, s;
   nn_t a, r1;
   len_t m;

   printf("nn_equal_m...");

   /* test copying and then modifiying yields non-equal integer */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state) + 1;

      a = nn_init(m);
      
      r1 = nn_init(m);
      
      nn_random(a, state, m);
         
      nn_copy(r1, a, m);

      s = randint(m, state);
      a[s] += 1;

      result = !nn_equal_m(r1, a, m);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
   }

   return result;
}

int test_equal(void)
{
   int result = 1;
   long i;
   nn_t a, b, r1, r2;
   len_t m1, m2;
   
   printf("nn_equal...");

   /* test that equal things compare equal */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state);
 
      r1 = nn_init(m1);
      r2 = nn_init(m1);

      nn_random(r1, state, m1);
      
      nn_copy(r2, r1, m1);

      result = nn_equal(r1, m1, r2, m1);

      if (!result)
      {
         printf("m1 = %ld\n", m1);
      }

      nn_clear(r1);
      nn_clear(r2);
   }

   /* test that not equal lengths compare not equal */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state) + 1;
      m2 = randint(m1, state);

      r1 = nn_init(m1);
      r2 = nn_init(m2);

      do {
         nn_random(r1, state, m1);
      } while (nn_normalise(r1, m1) != m1);
      
      do {
         nn_random(r2, state, m2);
      } while (nn_normalise(r2, m2) != m2);
      
      result = !nn_equal(r1, m1, r2, m2);

      if (!result)
      {
         printf("m1 = %ld, m2 = %ld\n", m1, m2);
      }

      nn_clear(r1);
      nn_clear(r2);
   }

   /* test that not equal values compare not equal */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state) + 1;
      
      r1 = nn_init(m1 + 1);
      a = nn_init(m1);
      b = nn_init(m1);

      do {
         nn_random(a, state, m1);
         nn_random(b, state, m1);
         nn_add_m(r1, a, b, m1);
      } while (r1[m1] || nn_normalise(b, m1) != m1);
      
      result = !nn_equal(r1, m1, a, m1);

      if (!result)
      {
         printf("m1 = %ld, m2 = %ld\n", m1, m2);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
   }

   return result;
}

int test_zero(void)
{
   int result = 1;
   long i;
   nn_t a;
   len_t m;

   printf("nn_zero...");

   /* test zeroing */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      nn_random(a, state, m);
         
      nn_zero(a, m);

      result = (nn_normalise(a, m) == 0);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
   }

   return result;
}

int test_normalise(void)
{
   int result = 1;
   long i, s1, s2;
   nn_t a, r1;
   len_t m;

   printf("nn_normalise...");

   /* test normalising then copying normalised part yields same integer */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state) + 1;

      a = nn_init(m);
      r1 = nn_init(m);
      
      nn_random(a, state, m);
      
      s1 = randint(m, state);       
      nn_zero(a + s1, m - s1);
      s2 = nn_normalise(a, m);

      nn_zero(r1, m);
      nn_copy(r1, a, s2);

      result = ((s1 >= s2) && ((s2 == 0) || (a[s2 - 1] != 0))
             && nn_equal_m(a, r1, m));

      if (!result)
      {
         printf("m = %ld, s1 = %ld, s2 = %ld\n", m, s1, s2);
      }

      nn_clear(a);
      nn_clear(r1);
   }

   return result;
}

int test_mul1(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_mul1...");

   /* test a * (c1 + c2) = a * c1 + a * c2 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      t1 = nn_init(m + 1);
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
   
      do
      {
         c1 = randword(state);
         c2 = randword(state);
      } while (c1 + c2 < c1);

      nn_mul1(t1, a, m, c1);
      nn_mul1(r1, a, m, c2);
      _nn_add_m(r1, r1, t1, m + 1);
      
      nn_mul1(r2, a, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
      }

      nn_clear(a);
      nn_clear(t1);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of mul1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      nn_random(a, state, m + n);
   
      c1 = randword(state);

      ci = _nn_mul1(r1, a, m, c1);
      nn_mul1_c(r1 + m, a + m, n, c1, ci);
      
      nn_mul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %ld, c1 = %lu\n", m, n, c1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_addmul1(void)
{
   int result = 1;
   long i;
   nn_t a, b, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_addmul1...");

   /* test a + b * (c1 + c2) = a + b * c1 + b * c2 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);
      
      t1 = nn_init(m + 1);
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
      nn_random(b, state, m);
   
      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      do
      {
         c1 = randword(state);
         c2 = randword(state);
      } while (c1 + c2 < c1);

      r1[m] = _nn_addmul1(r1, b, m, c1);
      nn_addmul1(r1, b, m, c2);
      
      r2[m] = _nn_addmul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %ld, c2 = %ld\n", m, c1, c2);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of addmul1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      nn_random(a, state, m + n);
      nn_random(r1, state, m + n);
      nn_copy(r2, r1, m + n);

      c1 = randword(state);

      ci = _nn_addmul1(r1, a, m, c1);
      r1[m + n] = _nn_addmul1_c(r1 + m, a + m, n, c1, ci);
      
      r2[m + n] = _nn_addmul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %ld, c1 = %ld\n", m, n, c1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_submul1(void)
{
   int result = 1;
   long i;
   nn_t a, b, r1, r2, t1;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_submul1...");

   /* test a - b * (c1 + c2) = a - b * c1 - b * c2 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);
      
      t1 = nn_init(m + 1);
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
      nn_random(b, state, m);
   
      nn_copy(r1, a, m);
      nn_copy(r2, a, m);

      do
      {
         c1 = randword(state);
         c2 = randword(state);
      } while (c1 + c2 < c1);

      r1[m] = _nn_submul1(r1, b, m, c1);
      nn_submul1(r1, b, m, c2);
      
      r2[m] = _nn_submul1(r2, b, m, c1 + c2);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %ld, c2 = %ld\n", m, c1, c2);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of submul1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      nn_random(a, state, m + n);
      nn_random(r1, state, m + n);
      nn_copy(r2, r1, m + n);

      c1 = randword(state);

      ci = _nn_submul1(r1, a, m, c1);
      r1[m + n] = _nn_submul1_c(r1 + m, a + m, n, c1, ci);
      
      r2[m + n] = _nn_submul1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %ld, c1 = %ld\n", m, n, c1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_add1(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_add1...");

   /* test a + c1 + c2 = a + c2 + c1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
   
      c1 = randword(state);
      c2 = randword(state);
      
      nn_add1(r1, a, m, c1);
      nn_add1(r1, r1, m, c2);
      
      nn_add1(r2, a, m, c2);
      nn_add1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of add1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      nn_random(a, state, m + n);
   
      c1 = randword(state);

      ci = _nn_add1(r1, a, m, c1);
      nn_add1(r1 + m, a + m, n, ci);
      
      nn_add1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %lu, c1 = %lu\n", m, n, c1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_sub1(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   word_t c1, c2, ci;
   len_t m, n;

   printf("nn_sub1...");

   /* test a - c1 - c2 = a - c2 - c1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
   
      c1 = randword(state);
      c2 = randword(state);
      
      nn_sub1(r1, a, m, c1);
      nn_sub1(r1, r1, m, c2);
      
      nn_sub1(r2, a, m, c2);
      nn_sub1(r2, r2, m, c1);

      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %lu, c2 = %lu\n", m, c1, c2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of sub1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);

      nn_random(a, state, m + n);
   
      c1 = randword(state);

      ci = _nn_sub1(r1, a, m, c1);
      nn_sub1(r1 + m, a + m, n, ci);
      
      nn_sub1(r2, a, m + n, c1);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %lu, c1 = %lu\n", m, n, c1);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test a + c1 - c1 = a */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m + 1);
      a[m] = 0;

      r1 = nn_init(m + 1);
      
      nn_random(a, state, m);
   
      c1 = randword(state);
      
      nn_add1(r1, a, m, c1);
      nn_sub1(r1, r1, m, c1);
      
      result = nn_equal_m(r1, a, m + 1);

      if (!result)
      {
         printf("m = %ld, c1 = %lu\n", m, c1);
      }

      nn_clear(a);
      nn_clear(r1);
   }

   return result;
}

int test_not(void)
{
   int result = 1;
   long i;
   nn_t a, r1;
   len_t m;

   printf("nn_not...");

   /* test not not a = a */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m);
      
      nn_random(a, state, m);
   
      nn_not(r1, a, m);
      nn_not(r1, r1, m);
      
      result = nn_equal_m(r1, a, m);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
   }

   return result;
}

int test_neg(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   len_t m, n;
   word_t ci;
   
   printf("nn_neg...");

   /* test neg a = not a + 1 */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);
      
      nn_random(a, state, m);
   
      nn_neg(r1, a, m);

      nn_not(r2, a, m);
      r2[m] = ~ (word_t) 0;
      nn_add1(r2, r2, m, 1);
      
      result = nn_equal_m(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   /* test chaining of neg */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n + 1);
      r2 = nn_init(m + n + 1);
      
      nn_random(a, state, m + n);
   
      nn_neg(r1, a, m + n);
      
      ci = _nn_neg(r2, a, m);
      nn_neg_c(r2 + m, a + m, n, ci);
      
      result = nn_equal_m(r1, r2, m + n + 1);

      if (!result)
      {
         printf("m = %ld, n = %ld\n", m, n);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_cmp_m(void)
{
   int result = 1;
   long i;
   nn_t a, b, r1, r2;
   len_t m1;
   
   printf("nn_cmp_m...");

   /* test that equal things compare equal */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state);
 
      r1 = nn_init(m1);
      r2 = nn_init(m1);

      nn_random(r1, state, m1);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp_m(r1, r2, m1) == 0);

      if (!result)
      {
         printf("m1 = %ld\n", m1);
      }

      nn_clear(r1);
      nn_clear(r2);
   }

   /* test that not equal values compare in the correct way */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state) + 1;
      
      r1 = nn_init(m1 + 1);
      a = nn_init(m1);
      b = nn_init(m1);

      do {
         nn_random(a, state, m1);
         nn_random(b, state, m1);
         nn_add_m(r1, a, b, m1);
      } while (r1[m1] || nn_normalise(b, m1) != m1);
      
      result = (nn_cmp_m(r1, a, m1) > 0 && nn_cmp_m(a, r1, m1) < 0);

      if (!result)
      {
         printf("m1 = %ld\n", m1);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
   }

   return result;
}

int test_cmp(void)
{
   int result = 1;
   long i;
   nn_t a, b, r1, r2;
   len_t m1, m2;
   
   printf("nn_cmp...");

   /* test that equal things compare equal */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state);
 
      r1 = nn_init(m1);
      r2 = nn_init(m1);

      nn_random(r1, state, m1);
      
      nn_copy(r2, r1, m1);

      result = (nn_cmp(r1, m1, r2, m1) == 0);

      if (!result)
      {
         printf("m1 = %ld\n", m1);
      }

      nn_clear(r1);
      nn_clear(r2);
   }

   /* test that not equal lengths compare in the correct way */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state) + 1;
      m2 = randint(m1, state);

      r1 = nn_init(m1);
      r2 = nn_init(m2);

      do {
         nn_random(r1, state, m1);
      } while (nn_normalise(r1, m1) != m1);
      
      do {
         nn_random(r2, state, m2);
      } while (nn_normalise(r2, m2) != m2);
      
      result = (nn_cmp(r1, m1, r2, m2) > 0 && nn_cmp(r2, m2, r1, m1) < 0);

      if (!result)
      {
         printf("m1 = %ld, m2 = %ld\n", m1, m2);
      }

      nn_clear(r1);
      nn_clear(r2);
   }

   /* test that not equal values compare in the correct way */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m1 = randint(100, state) + 1;
      
      r1 = nn_init(m1 + 1);
      a = nn_init(m1);
      b = nn_init(m1);

      do {
         nn_random(a, state, m1);
         nn_random(b, state, m1);
         nn_add_m(r1, a, b, m1);
      } while (r1[m1] || nn_normalise(b, m1) != m1);
      
      result = (nn_cmp(r1, m1, a, m1) > 0 && nn_cmp(a, m1, r1, m1) < 0);

      if (!result)
      {
         printf("m1 = %ld, m2 = %ld\n", m1, m2);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
   }

   return result;
}

int test_divrem1_simple(void)
{
   int result = 1;
   long i;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, ci, r;

   printf("nn_divrem1_simple...");

   /* test that a = q * d + r */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
 
      r1 = nn_init(m);
      a = nn_init(m);
      q = nn_init(m);

      nn_random(a, state, m);

      do {
         d = randword(state);
      } while (d == 0);

      r = _nn_divrem1_simple(q, a, m, d);

      ci = _nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result)
      {
         printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
      }

      nn_clear(r1);
      nn_clear(a);
      nn_clear(q);
   }

   /* test chaining of divrem1_simple */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n);
      r2 = nn_init(m + n);

      do {
         d = randword(state);
      } while (d == 0);

      nn_random(a, state, m + n);
         
      ci = _nn_divrem1_simple(r1 + n, a + n, m, d);
      _nn_divrem1_simple_c(r1, a, n, d, ci);
      
      _nn_divrem1_simple(r2, a, m + n, d);

      result = nn_equal_m(r1, r2, m + n);

      if (!result)
      {
         printf("m = %ld, n = %ld, d = %lu\n", m, n, d);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_divrem1_preinv(void)
{
   int result = 1;
   long i;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, dnorm, ci, r, rem1, rem2;
   preinv1_t inv;

   printf("nn_divrem1_preinv...");

   /* test that a = q * d + r */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
 
      r1 = nn_init(m);
      a = nn_init(m);
      q = nn_init(m);

      nn_random(a, state, m);

      do {
         d = randword(state);
      } while (d == 0);

      dnorm = precompute_inverse1(&inv, d);

      r = _nn_divrem1_preinv(q, a, m, dnorm, inv);
      r >>= inv.norm;

      ci = _nn_mul1_c(r1, q, m, d, r);

      result = (nn_equal_m(r1, a, m) && ci == 0);

      if (!result)
      {
         printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
      }

      nn_clear(r1);
      nn_clear(a);
      nn_clear(q);
   }

   /* test chaining of divrem1_preinv */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n);
      r2 = nn_init(m + n);

      do {
         d = randword(state);
      } while (d == 0);

      dnorm = precompute_inverse1(&inv, d);

      nn_random(a, state, m + n);
         
      ci = _nn_divrem1_preinv(r1 + n, a + n, m, dnorm, inv);
      rem1 = _nn_divrem1_preinv_c(r1, a, n, dnorm, inv, ci);
      
      rem2 = _nn_divrem1_preinv(r2, a, m + n, dnorm, inv);

      result = (nn_equal_m(r1, r2, m + n) && rem1 == rem2);

      if (!result)
      {
         printf("m = %ld, n = %ld, d = %lu, rem1 = %lu, rem2 = %lu\n", 
            m, n, d, rem1, rem2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_divrem_hensel1_preinv(void)
{
   int result = 1;
   long i;
   nn_t a, q, r1, r2;
   len_t m, n;
   word_t d, ci, r, rem1, rem2;
   hensel_preinv1_t inv;

   printf("nn_divrem_hensel1_preinv...");

   /* test that a + B^m * r = q * d */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
 
      r1 = nn_init(m);
      a = nn_init(m);
      q = nn_init(m);

      nn_random(a, state, m);

      d = randword(state);
      d |= (word_t) 1; /* d must be odd */

      precompute_hensel_inverse1(&inv, d);

      r = _nn_divrem_hensel1_preinv(q, a, m, d, inv);
      
      ci = _nn_mul1(r1, q, m, d);

      result = (nn_equal_m(r1, a, m) && ci == r);

      if (!result)
      {
         printf("m = %ld, ci = %lu, r = %lu\n", m, ci, r);
      }

      nn_clear(r1);
      nn_clear(a);
      nn_clear(q);
   }

   /* test chaining of divrem_hensel1_preinv */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      r1 = nn_init(m + n);
      r2 = nn_init(m + n);

      d = randword(state);
      d |= (word_t) 1; /* d must be odd */

      precompute_hensel_inverse1(&inv, d);

      nn_random(a, state, m + n);
         
      ci = _nn_divrem_hensel1_preinv(r1, a, m, d, inv);
      rem1 = _nn_divrem_hensel1_preinv_c(r1 + m, a + m, n, d, inv, ci);
      
      rem2 = _nn_divrem_hensel1_preinv(r2, a, m + n, d, inv);

      result = (nn_equal_m(r1, r2, m + n) && (rem1 == rem2));

      if (!result)
      {
         printf("m = %ld, n = %ld, c = %lu, rem1 = %lu, rem2 = %lu\n", 
            m, n, d, rem1, rem2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_mod1_preinv(void)
{
   int result = 1;
   long i;
   nn_t a, q;
   len_t m, n;
   word_t d, dnorm, ci, rem1, rem2;
   preinv1_t inv;
   mod_preinv1_t minv;

   printf("nn_mod1_preinv...");

   /* test that divrem1 and mod1 return same remainder */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
 
      a = nn_init(m);
      q = nn_init(m);

      nn_random(a, state, m);

      do {
         d = randword(state);
      } while (d == 0);

      dnorm = precompute_inverse1(&inv, d);

      rem1 = _nn_divrem1_preinv(q, a, m, dnorm, inv);
      rem1 >>= inv.norm;

      precompute_mod_inverse1(&minv, d);

      rem2 = _nn_mod1_preinv(a, m, d, minv);  
      
      result = (rem1 == rem2);

      if (!result)
      {
         printf("m = %ld, rem1 = %lu, rem2 = %lu\n", m, rem1, rem2);
      }

      nn_clear(a);
      nn_clear(q);
   }

   /* test chaining of mod1_preinv */
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);
      n = randint(100, state);

      a = nn_init(m + n);
      
      do {
         d = randword(state);
      } while (d == 0);

      precompute_mod_inverse1(&minv, d);

      nn_random(a, state, m + n);
         
      ci = _nn_mod1_preinv(a + n, m, d, minv);
      rem1 = _nn_mod1_preinv_c(a, n, d, minv, ci);
      
      rem2 = _nn_mod1_preinv(a, m + n, d, minv);

      result = (rem1 == rem2);

      if (!result)
      {
         printf("m = %ld, n = %ld, d = %ld, rem1 = %lu, rem2 = %lu\n", 
            m, n, d, rem1, rem2);
      }

      nn_clear(a);
   }

   return result;
}

int test_generics(void)
{
   int result = 1;
   len_t m1;
   obj_t * w1, * w2, * w3, * w4, * w5, * w6, * w7, * w8, * n1, * n2, * n3, * n4;

   printf("generics...");

   TEST_START(ITER) 
   {
      m1 = randint(100, state);
      
      new_objs(WORD, &w1, &w2, &w3, &w4, &w5, &w6, &w7, &w8, NULL);
      new_objs(NN, &n1, &n2, &n3, &n4, NULL);
      
      randoms(ANY, state, w1, w2, NULL);
      randoms(ODD, state, w3, NULL);
      randoms(NONZERO, state, w4, NULL);

      randoms_upto(w4, ANY, state, w5, w6, w7, w8, NULL);

      randoms_of_len(m1, ANY, state, n1, n2, n3, n4, NULL);
   } TEST_END;

   return result;
}

#define RUN(xxx) \
   do { \
      if (xxx()) \
      { \
         printf("PASS\n"); \
         pass++; \
      } else \
      { \
         printf("FAIL\n"); \
         fail++; \
      } \
   } while (0)

int main(void)
{
   long pass = 0;
   long fail = 0;
   
   randinit(state);

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
   RUN(test_generics);

   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   randclear(state);

   return 0;
}

