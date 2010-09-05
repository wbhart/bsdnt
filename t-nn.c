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
   for (i = 0; i < ITER; i++)
   {
      m = randint(100, state);

      a = nn_init(m);
      b = nn_init(m);
      c = nn_init(m);

      r1 = malloc((m + 1)*sizeof(word_t));
      r2 = malloc((m + 1)*sizeof(word_t));

      nn_random(a, state, m);
      nn_random(b, state, m);
      nn_random(c, state, m);

      nn_add_m(r1, a, b, m);
      nn_add_m(r1, r1, c, m);

      nn_add_m(r2, b, c, m);
      nn_add_m(r2, r2, a, m);

      if (!nn_equal(r1, r2, m + 1))
      {
         printf("FAIL\n");
         printf("m = %ld\n", m);
         abort();
      }

      nn_clear(a);
      nn_clear(b);
      nn_clear(c);
   }
}

#define RUN(xxx) \
   do { \
      if (xxx()) pass++; \
      else fail++; \
   } while (0)

int main(void)
{
   long pass = 0;
   long fail = 0;
   
   randinit(state);
   
   RUN(test_add);

   printf("%ld tests of %ld pass.\n", pass, pass + fail);

   randclear(state);
}

