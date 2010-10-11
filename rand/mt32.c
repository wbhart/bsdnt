/* 
-------------------------------------------------------------------------
   Copyright (C) 2010, Brian Gladman. Modifications for use in BSDNT
   Copyright (C) 2010, William Hart.  "   "   "   "   "   "   "   "
   
   Original header:
-------------------------------------------------------------------------
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
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
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include "../helper.h"

#if WORD_BITS == 32

#include "internal_rand.h"

#define NN 624

typedef struct
{   
   word_t mt[NN];
	word_t mag01[2];
	word_t mti;
} mt_ctx;

#define CTX(x) ((mt_ctx*)(x))

#define MM 397
#define MATRIX_A 0x9908b0dfUL /* constant vector a */
#define UM 0x80000000UL /* most significant w-r bits */
#define LM 0x7fffffffUL /* least significant r bits */

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

   for (IX(CTX(c)) = 1; IX(CTX(c)) < NN; IX(CTX(c))++) 
   {
      MT(CTX(c))[IX(CTX(c))] = 
	   (1812433253UL * (MT(CTX(c))[IX(CTX(c)) - 1] 
                    ^ (MT(CTX(c))[IX(CTX(c)) - 1] >> 30)) + IX(CTX(c))); 
   }
}

/* 
   initialize by an array with array-length 
   init_key is the array for initializing keys 
   key_length is its length 
*/
void init_by_array(word_t * init_key, word_t key_length, rand_t c)
{
    word_t i = 1, j = 0, k = (NN > key_length ? NN : key_length);

    init_genrand(19650218UL);

    for ( ; k; k--) 
    {
        MT(CTX(c))[i] = (MT(CTX(c))[i] ^ ((MT(CTX(c))[i - 1] 
                      ^ (MT(CTX(c))[i - 1] >> 30)) * 1664525UL))
                      + init_key[j] + j; /* non linear */
        
        i++; j++;
        
        if (i >= NN) { MT(CTX(c))[0] = MT(CTX(c))[NN - 1]; i = 1; }
        if (j >= key_length) j = 0;
    }
    
    for (k = NN - 1; k; k--) 
    {
        MT(CTX(c))[i] = (MT(CTX(c))[i] ^ ((MT(CTX(c))[i - 1] 
                      ^ (MT(CTX(c))[i - 1] >> 30)) * 1566083941UL)) 
                      - i; /* non linear */
        
        i++;
        
        if (i >= NN) { MT(CTX(c))[0] = MT(CTX(c))[NN - 1]; i = 1; }
    }

    MT(CTX(c))[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number in [0, 2^32-1]-interval */

word_t mt_word(rand_t c)
{
   word_t kk, y;

   if (IX(CTX(c)) >= NN)  /* generate NN words at one time */
   {
      if (IX(CTX(c)) == NN + 1)   /* if init_genrand() has not been called, */
         init_genrand(5489UL); /* a default initial seed is used */

      for (kk = 0; kk < NN - MM; kk++) 
      {
         y = (MT(CTX(c))[kk] & UM) | (MT(CTX(c))[kk + 1] & LM);
         MT(CTX(c))[kk] = MT(CTX(c))[kk + MM] 
            ^ (y >> 1) ^ MG(CTX(c))[y & 0x1UL];
      }
        
      for ( ; kk < NN - 1; kk++) 
      {
         y = (MT(CTX(c))[kk] & UM) | (MT(CTX(c))[kk + 1] & LM);
         MT(CTX(c))[kk] = MT(CTX(c))[kk + (MM - NN)] 
            ^ (y >> 1) ^ MG(CTX(c))[y & 0x1UL];
      }
       
      y = (MT(CTX(c))[NN - 1] & UM) | (MT(CTX(c))[0] & LM);
      MT(CTX(c))[NN - 1] = MT(CTX(c))[MM - 1] 
         ^ (y >> 1) ^ MG(CTX(c))[y & 0x1UL];

      IX(CTX(c)) = 0;
   }
  
   y = MT(CTX(c))[IX(CTX(c))++];

   /* Tempering */
   y ^= (y >> 11);
   y ^= (y << 7) & 0x9d2c5680UL;
   y ^= (y << 15) & 0xefc60000UL;
   y ^= (y >> 18);

   return y;
}

#endif