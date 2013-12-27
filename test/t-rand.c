/* 
  Copyright (C) 2010, Brian Gladman

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
#include <string.h>
#include "rand.h"
#include "nn.h"
#include "test.h"
#include "sha1.h"

rand_t state;

#define N_ITER   10000
#define BUF_SIZE  1000

static unsigned char digest[3][SHA1_DIGEST_SIZE] = 
{
#if WORD_BITS == 32
    { 0xb4, 0x18, 0x46, 0xd0, 0x37, 0xa4, 0xf7, 0xec, 0x18, 0x8b, 
      0xe4, 0x7c, 0xeb, 0x27, 0xaa, 0x2f, 0x56, 0x4d, 0x8a, 0xd7 },
    { 0xb5, 0x37, 0xfc, 0x51, 0x3f, 0x54, 0xe3, 0xd1, 0x72, 0xae, 
      0xa7, 0x2a, 0x1b, 0xfd, 0xbb, 0xc3, 0xc8, 0x80, 0x55, 0x2a },
    { 0x7e, 0xe1, 0x53, 0x40, 0x93, 0x94, 0x85, 0x60, 0xb7, 0xb7, 
      0xc1, 0xa2, 0x28, 0x7f, 0xb3, 0x1c, 0xe2, 0xf1, 0x9d, 0x11 }
#else
    { 0xcd, 0x39, 0xd0, 0x88, 0x77, 0x8a, 0x69, 0x3f, 0xbf, 0xfa, 
      0x37, 0xe3, 0xa3, 0x07, 0xcb, 0x7b, 0x43, 0x94, 0x88, 0xc6 },
    { 0xf4, 0x5b, 0x86, 0xcc, 0x3b, 0x3c, 0x4a, 0xeb, 0xeb, 0xbe, 
      0xe9, 0x46, 0x33, 0xba, 0xb3, 0x82, 0x76, 0x14, 0xba, 0x4f },
    { 0x6f, 0xa1, 0x43, 0xb5, 0x43, 0xea, 0x56, 0x30, 0xc7, 0x25, 
      0x69, 0xf0, 0xbc, 0xd8, 0xbf, 0xfa, 0x50, 0xd5, 0x1c, 0x38 }
#endif
};

int test_rand(void)
{   unsigned int k, r = (unsigned int) RAND_START;
    long pass = 0, fail = 0;
    sha1_ctx ctx[1];
    rand_t state[1];
    unsigned char buf[BUF_SIZE * sizeof(word_t)];
    unsigned char hash[SHA1_DIGEST_SIZE];

    while (++r != (unsigned int) RAND_END) /* for each pseudo random function */
    {   
		int i, j;

        sha1_begin(ctx);
        *state = set_rand_algorithm((random_algorithm) r);
        state->ctx = state->init();

        for (i = 0; i < N_ITER; ++i) /* hash this many buffers worth */
        {
            for (j = 0; j < BUF_SIZE; ++j) /* fill a buffer with words */
            {   
				    word_t v = state->word(state->ctx);

                for (k = 0; k < sizeof(word_t); ++k) /* for each byte in a word */
                {
                    buf[j * sizeof(word_t) + k] = (unsigned char) v;
                    v >>= 8;
                }
            }

            sha1_hash((unsigned char *) buf, BUF_SIZE * sizeof(word_t), ctx);
        }

        sha1_end(hash, ctx);
        state->clear(state->ctx);
        
		  if (memcmp(hash, digest[r - 1], SHA1_DIGEST_SIZE)) /* check against a precomputed hash */
        {
            printf("Error in the %s %2d-bit Pseudo Random Number Generator\n", state->name, WORD_BITS);
            fail++;
        } else
        {
            printf("%s...PASS\n", state->name);
            pass++;
        }
    }

    printf("%ld of %ld tests pass.\n", pass, pass + fail);
    return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting rand functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_rand();
   
   randclear(state);

   return ret;
}
