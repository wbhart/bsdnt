#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "test.h"

#define ITER 30000

rand_t state;

int test_mul_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, c, r1, r2, s;

   printf("mul_classical...");

   TEST_START(ITER) /* test a * (b + c) = a * b + a * c */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n + 1, ANY, state, &s, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      do {
         randoms_of_len(n, ANY, state, &b, &c, NULL);
         nn_s_add_m(s, b, c, n);
      } while (s[n]);
      
      nn_s_mul_classical(r1, a, m, b, n);
      nn_s_mul_classical(r2, a, m, c, n);
      nn_add_m(r2, r2, r1, m + n);

      nn_s_mul_classical(r1, a, m, s, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   return result;
}

int main(void)
{
   long pass = 0;
   long fail = 0;
   
   randinit(state);

   RUN(test_mul_classical);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   randclear(state);

   return 0;
}


