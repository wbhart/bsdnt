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
#include <time.h>
#include <math.h>
#include "nn.h"
#include "test.h"

#undef ITER
#define ITER 10000

rand_t state;

void time_div_divconquer(void)
{
   nn_t a, a2, b, q1, q2;
   len_t size;
   word_t inv;
   long count, i;
   clock_t t;

   TMP_INIT;

   for (size = 370; size < 1000; size = (long) ceil(size*1.1))
   {
      TMP_START;
      
      a = TMP_ALLOC(2*size - 1);
      a2 = TMP_ALLOC(2*size - 1);
      b = TMP_ALLOC(size);
      q1 = TMP_ALLOC(size);
      q2 = TMP_ALLOC(size);
               
      printf("size = %ld: ", size);

      t = clock();
      for (i = 0; i < 10; i++)
      {
         randoms_of_len(size, ANY, state, &b, NULL);
         b[size - 1] |= (1UL<<(WORD_BITS - 1));
         randoms_of_len(2*size - 1, ANY, state, &a, NULL);
         inv = precompute_inverse2(b[size - 1], b[size - 2]);
         
         for (count = 0; count < ITER/10; count++)
         {
            nn_copy(a2, a, 2*size - 1);
            nn_divrem_classical_preinv_c(q1, a2, 2*size - 1, b, size, inv, 0);
         }
      }
      t = clock() - t;

      printf("classical = %gs, ", ((double) t)/CLOCKS_PER_SEC/ITER);

      t = clock();
      for (i = 0; i < 10; i++)
      {
         randoms_of_len(size, ANY, state, &b, NULL);
         b[size - 1] |= (1UL<<(WORD_BITS - 1));
         randoms_of_len(2*size - 1, ANY, state, &a, NULL);
         inv = precompute_inverse2(b[size - 1], b[size - 2]);
         
         for (count = 0; count < ITER/10; count++)
         {
            nn_copy(a2, a, 2*size - 1);
            nn_div_divconquer_preinv_c(q2, a2, 2*size - 1, b, size, inv, 0);
         }
      }
      t = clock() - t;

      printf("divconquer = %gs\n", ((double) t)/CLOCKS_PER_SEC/ITER);
     
      TMP_END;
   }
}

int main(void)
{
   printf("\nTiming nn_div_divconquer vs nn_divrem_classical:\n");
   
   randinit(&state);
   
   time_div_divconquer();

   randclear(state);

   return 0;
}

