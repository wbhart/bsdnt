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
#define ITER 20000

rand_t state;

void time_mulmid_kara(void)
{
   nn_t a, b, r1, r2;
   len_t size;
   long count;
   clock_t t;

   TMP_INIT;

   for (size = 2; size < 1000; size = (long) ceil(size*1.1))
   {
      TMP_START;
      
      a = TMP_ALLOC(2*size);
      b = TMP_ALLOC(size);
      r1 = TMP_ALLOC(3*size);
      r2 = TMP_ALLOC(3*size);
      
      randoms_of_len(2*size, ANY, state, &a, NULL);
      randoms_of_len(size, ANY, state, &b, NULL);
      
      printf("size = %ld: ", size);

      t = clock();
      for (count = 0; count < ITER; count++)
         nn_mulmid_classical(r1 + size, r1, a, 2*size, b, size);
      t = clock() - t;

      printf("classical = %gs, ", ((double) t)/CLOCKS_PER_SEC/ITER);

      t = clock();
      for (count = 0; count < ITER; count++)
         nn_mulmid_kara(r2 + size, r2, a, 2*size, b, size);
      t = clock() - t;

      printf("kara = %gs\n", ((double) t)/CLOCKS_PER_SEC/ITER);
     
      TMP_END;
   }
}

int main(void)
{
   printf("\nTiming nn_mulmid_kara vs nn_mulmid_classical:\n");
   
   randinit(&state);
   
   time_mulmid_kara();

   randclear(state);

   return 0;
}

