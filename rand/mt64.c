/* 
-------------------------------------------------------------------------
   Copyright (C) 2010, Brian Gladman. Modifications for use in BSDNT

   Original header:
-------------------------------------------------------------------------
   A C-program for MT19937-64 (2004/9/29 version). Coded by Takuji 
   Nishimura and Makoto Matsumoto.  This is a 64-bit version of 
   Mersenne Twister pseudorandom number generator.

   Before using, initialize the state by using init_genrand64(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT 
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------------------------------
*/

#include "../helper.h"

#if WORD_BITS == 64

#include "internal_rand.h"

#define NN 312

typedef struct
{   
   word_t mt[NN];
	word_t mag01[2];
	word_t mti;
} mt_ctx;

#define CTX(x) ((mt_ctx*)(x))

#define MM 156
#define MATRIX_A WORD_CONST(0xb5026f5aa96619e9) /* constant vector a */
#define LM WORD_CONST(0x7fffffff)

#define MT(x)	((x)->mt)
#define IX(x)	((x)->mti)
#define MG(x)	((x)->mag01)

rand_t mt_start(void)
{
	rand_t c = malloc(sizeof(mt_ctx));

	IX(CTX(c)) = NN + 1;
	MG(CTX(c))[0] = 0;
	MG(CTX(c))[1] = MATRIX_A;

	return c;
}

void mt_end(rand_t ctx)
{
	free(ctx);
}

void init_genrand(word_t seed, rand_t c)
{
    MT(CTX(c))[0] = seed;

    for (IX(CTX(c)) = 1 ; IX(CTX(c)) < NN ; ++(IX(CTX(c)))) 
    {
       MT(CTX(c))[IX(CTX(c))] = (WORD_CONST(6364136223846793005) * 
		         (MT(CTX(c))[IX(CTX(c)) - 1] ^ 
               (MT(CTX(c))[IX(CTX(c)) - 1] >> 62)) + IX(CTX(c)));
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */

void init_by_array(word_t * init_key, word_t key_length, rand_t c)
{
   word_t i = 1, j = 0, k = NN > key_length ? NN : key_length;
   init_genrand(WORD_CONST(19650218), CTX(c));
   
   for( ; k ; --k ) 
	{
		MT(CTX(c))[i] = (MT(CTX(c))[i] ^ ((MT(CTX(c))[i - 1] ^ 
			(MT(CTX(c))[i - 1] >> 62)) * WORD_CONST(3935559000370003845))) 
         + init_key[j] + j; /* non linear */
      
      i++; j++;

      if(i >= NN) 
		{ 
			MT(CTX(c))[0] = MT(CTX(c))[NN - 1];
			i = 1; 
		}
        
		if(j >= key_length) 
			j = 0;
   }

   for (k = NN - 1 ; k ; --k) 
   {
      MT(CTX(c))[i] = (MT(CTX(c))[i] ^ ((MT(CTX(c))[i - 1] ^ 
		  (MT(CTX(c))[i - 1] >> 62)) * WORD_CONST(2862933555777941757))) 
        - i; /* non linear */
        
      i++;
        
		if(i >= NN) 
		{ 
			MT(CTX(c))[0] = MT(CTX(c))[NN - 1];
			i = 1; 
      }
   }

   MT(CTX(c))[0] = WORD_CONST(1) << 63; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number in [0, 2^64-1]-interval */

word_t mt_word(rand_t c)
{
   word_t i, x;

	if (IX(CTX(c)) >= NN) 
	{
      if (IX(CTX(c)) == NN + 1) /* if init_genrand() has not been called, */
         init_genrand(WORD_CONST(5489), (CTX(c))); /* a default initial seed is used */

      for( i = 0 ; i < NN - MM ; ++i ) 
		{
			x = (MT(CTX(c))[i] & ~LM) | (MT(CTX(c))[i + 1] & LM);
         MT(CTX(c))[i] = MT(CTX(c))[i + MM] ^ (x >> 1) 
                       ^ MG(CTX(c))[x & 1];
      }
        
      for( ; i < NN - 1 ; ++i ) 
		{
         x = (MT(CTX(c))[i] & ~LM) | (MT(CTX(c))[i + 1] & LM);
          MT(CTX(c))[i] = MT(CTX(c))[i + (MM - NN)] ^ (x >> 1) 
                        ^ MG(CTX(c))[x & 1];
      }
        
      x = (MT(CTX(c))[NN - 1] & ~LM) | (MT(CTX(c))[0] & LM);
      MT(CTX(c))[NN - 1] = MT(CTX(c))[MM - 1] ^ (x >> 1) 
                         ^ MG(CTX(c))[x & 1];

      IX(CTX(c)) = 0;
   }
  
   /* Tempering */
   x = MT(CTX(c))[IX(CTX(c))++];
   x ^= (x >> 29) & WORD_CONST(0x5555555555555555);
   x ^= (x << 17) & WORD_CONST(0x71D67FFFEDA60000);
   x ^= (x << 37) & WORD_CONST(0xFFF7EEE000000000);
   x ^= (x >> 43);

   return x;
}

#endif