#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "test.h"

#define ITER 50000

rand_t state;

int test_mul_classical(void)
{
   int result = 1;
   len_t m;
   nn_t a, b, c;

   printf("mul_classical...");

   TEST_START(ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, &c, NULL);
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


