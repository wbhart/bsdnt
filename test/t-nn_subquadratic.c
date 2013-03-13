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
#define ITER 30000

rand_t state;

int test_mul_kara(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2;

   printf("mul_kara...");

   TEST_START(1, ITER) /* test mul_kara gives same as mul_classical */
   {
      randoms_upto(60, NONZERO, state, &n, NULL);
      randoms_upto((n + 1)/3 + 1, ANY, state, &m, NULL);
      
      randoms_of_len(m + n + 1, ANY, state, &a, NULL);
      randoms_of_len(n + 1,     ANY, state, &b, NULL);
      
      randoms_of_len(m + 2*n + 2, ANY, state, &r1, &r2, NULL);
      
      nn_mul_classical(r1, a, m + n + 1, b, n + 1);
      nn_mul_kara(r2, a, m + n + 1, b, n + 1);
      
      result = nn_equal_m(r1, r2, m + 2*n + 2);

      if (!result) 
      {
         print_debug(a, m + n + 1); print_debug(b, n + 1);
         print_debug_diff(r1, r2, m + 2*n + 2);
      }
   } TEST_END;

   return result;
}

int test_subquadratic(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_mul_kara);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting nn_subquadratic functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_subquadratic();

   randclear(state);

   return ret;
}

