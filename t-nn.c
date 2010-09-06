#include <stdio.h>
#include <stdlib.h>
#include "nn.h"

#define ITER 100000

rand_t state;

int test_add(void)
{
   int result = 1;
   long i;
   nn_t a, b, c, r1, r2;
   len_t m;

   printf("nn_add...");

   // test (a + b) + c = a + (b + c)
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);
      c = nn_init(m);

      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
      nn_random(b, state, m);
      nn_random(c, state, m);

      nn_add_m(r1, a, b, m);
      nn_add_m(r1, r1, c, m);

      nn_add_m(r2, b, c, m);
      nn_add_m(r2, r2, a, m);

      result = nn_equal(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(c);
      nn_clear(r1);
      nn_clear(r2);
   }

   return result;
}

int test_sub(void)
{
   int result = 1;
   long i;
   nn_t a, b, c, r1, r2;
   len_t m;

   printf("nn_sub...");

   // test (a - b) - c = (a - c) - b
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);
      c = nn_init(m);

      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
      nn_random(b, state, m);
      nn_random(c, state, m);

      nn_sub_m(r1, a, b, m);
      nn_sub_m(r1, r1, c, m);

      nn_sub_m(r2, a, c, m);
      nn_sub_m(r2, r2, b, m);

      result = nn_equal(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(c);
      nn_clear(r1);
      nn_clear(r2);
   }

   // test (a + b) - b = a
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);

      r1 = nn_init(m + 1);
      
      nn_random(a, state, m);
      nn_random(b, state, m);
      
      nn_add_m(r1, a, b, m);
      nn_sub_m(r1, r1, b, m);

      result = (nn_equal(r1, a, m) && (r1[m] == 0));

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(r1);
   }

   return result;
}

int test_shl(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m;

   printf("nn_shl...");

   // test (a << sh1) << sh2 = (a << sh2) << sh1
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

      result = nn_equal(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   // test a << 1 = a + a
   for (i = 0; i < ITER && result == 1; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      
      r1 = nn_init(m + 1);
      r2 = nn_init(m + 1);

      nn_random(a, state, m);
         
      nn_shl(r1, a, m, 1);
      
      nn_add_m(r2, a, a, m);
      
      result = nn_equal(r1, r2, m + 1);

      if (!result)
      {
         printf("m = %ld\n", m);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }


   return result;
}

int test_shr(void)
{
   int result = 1;
   long i;
   nn_t a, r1, r2;
   bits_t sh1, sh2;
   len_t m;

   printf("nn_shr...");

   // test (a >> sh1) >> sh2 = (a >> sh2) >> sh1
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

      result = nn_equal(r1, r2, m);

      if (!result)
      {
         printf("m = %ld, sh1 = %ld, sh2 = %ld\n", m, sh1, sh2);
      }

      nn_clear(a);
      nn_clear(r1);
      nn_clear(r2);
   }

   // test (a << sh1) >> sh1 = a
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
      
      result = nn_equal(a, r2, m);

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
   
   RUN(test_add);
   RUN(test_sub);
   RUN(test_shl);
   RUN(test_shr);

   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   randclear(state);

   return 0;
}

