/* 
-------------------------------------------------------------------------
   Copyright (C) 2010, Brian Gladman. Modifications for use in BSDNT
   Copyright (C) 2010, William Hart.  "   "   "   "   "   "   "   "
   
-------------------------------------------------------------------------
   Original header (32 bit):

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

   -------------------------------------------------------------------------
   Original header (64 bit):

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

#include "helper.h"
#include <stdlib.h>
#include "internal_rand.h"

#if WORD_BITS == 32

#define NN 624

typedef struct
{   
   word_t mt[NN];
	word_t mag01[2];
	word_t mti;
} mt_ctx;

#define CTX(x) ((mt_ctx*)(x))

#define MM 397
#define MATRIX_A WORD(0x9908b0df) /* constant vector a */
#define UM WORD(0x80000000) /* most significant w-r bits */
#define LM WORD(0x7fffffff) /* least significant r bits */

#define MT(x)	(CTX(x)->mt)
#define IX(x)	(CTX(x)->mti)
#define MG(x)	(CTX(x)->mag01)

rand_ctx mt_init(void)
{
	rand_ctx c = malloc(sizeof(mt_ctx));

	IX(c) = NN + 1;
	MG(c)[0] = 0;
	MG(c)[1] = MATRIX_A;

	return c;
}

void mt_clear(rand_ctx ctx)
{
	free(ctx);
}

void init_genrand(word_t seed, rand_ctx c)
{
   MT(c)[0] = seed;

   for (IX(c) = 1; IX(c) < NN; IX(c)++) 
   {
      MT(c)[IX(c)] = 
	   (WORD(1812433253) * (MT(c)[IX(c) - 1] 
                    ^ (MT(c)[IX(c) - 1] >> 30)) + IX(c)); 
   }
}

/* 
   initialize by an array with array-length 
   init_key is the array for initializing keys 
   key_length is its length 
*/
void init_by_array(word_t * init_key, word_t key_length, rand_ctx c)
{
    word_t i = 1, j = 0, k = (NN > key_length ? NN : key_length);

    init_genrand(WORD(19650218), CTX(c));

    for ( ; k; k--) 
    {
        MT(c)[i] = (MT(c)[i] ^ ((MT(c)[i - 1] 
                      ^ (MT(c)[i - 1] >> 30)) * WORD(1664525)))
                      + init_key[j] + j; /* non linear */
        
        i++; j++;
        
        if (i >= NN) { MT(c)[0] = MT(c)[NN - 1]; i = 1; }
        if (j >= key_length) j = 0;
    }
    
    for (k = NN - 1; k; k--) 
    {
        MT(c)[i] = (MT(c)[i] ^ ((MT(c)[i - 1] 
                      ^ (MT(c)[i - 1] >> 30)) * WORD(1566083941))) 
                      - i; /* non linear */
        
        i++;
        
        if (i >= NN) { MT(c)[0] = MT(c)[NN - 1]; i = 1; }
    }

    MT(c)[0] = WORD(0x80000000); /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number in [0, 2^32-1]-interval */

word_t mt_word(rand_ctx c)
{
   word_t kk, y;

   if (IX(c) >= NN)  /* generate NN words at one time */
   {
      if (IX(c) == NN + 1)   /* if init_genrand() has not been called, */
         init_genrand(WORD(5489), CTX(c)); /* a default initial seed is used */

      for (kk = 0; kk < NN - MM; kk++) 
      {
         y = (MT(c)[kk] & UM) | (MT(c)[kk + 1] & LM);
         MT(c)[kk] = MT(c)[kk + MM] 
            ^ (y >> 1) ^ MG(c)[y & WORD(0x1)];
      }
        
      for ( ; kk < NN - 1; kk++) 
      {
         y = (MT(c)[kk] & UM) | (MT(c)[kk + 1] & LM);
         MT(c)[kk] = MT(c)[kk + (MM - NN)] 
            ^ (y >> 1) ^ MG(c)[y & WORD(0x1)];
      }
       
      y = (MT(c)[NN - 1] & UM) | (MT(c)[0] & LM);
      MT(c)[NN - 1] = MT(c)[MM - 1] 
         ^ (y >> 1) ^ MG(c)[y & WORD(0x1)];

      IX(c) = 0;
   }
  
   y = MT(c)[IX(c)++];

   /* Tempering */
   y ^= (y >> 11);
   y ^= (y << 7) & WORD(0x9d2c5680);
   y ^= (y << 15) & WORD(0xefc60000);
   y ^= (y >> 18);

   return y;
}

#elif WORD_BITS == 64

#define NN 312

typedef struct
{   
   word_t mt[NN];
	word_t mag01[2];
	word_t mti;
} mt_ctx;

#define CTX(x) ((mt_ctx*)(x))

#define MM 156
#define MATRIX_A WORD(0xb5026f5aa96619e9) /* constant vector a */
#define LM WORD(0x7fffffff)

#define MT(x)	(CTX(x)->mt)
#define IX(x)	(CTX(x)->mti)
#define MG(x)	(CTX(x)->mag01)

rand_ctx mt_init(void)
{
	rand_ctx c = malloc(sizeof(mt_ctx));

	IX(c) = NN + 1;
	MG(c)[0] = 0;
	MG(c)[1] = MATRIX_A;

	return c;
}

void mt_clear(rand_ctx c)
{
	free(c);
}

void init_genrand(word_t seed, rand_ctx c)
{
    MT(c)[0] = seed;

    for (IX(c) = 1 ; IX(c) < NN ; ++(IX(c))) 
    {
       MT(c)[IX(c)] = (WORD(6364136223846793005) * 
		         (MT(c)[IX(c) - 1] ^ 
               (MT(c)[IX(c) - 1] >> 62)) + IX(c));
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */

void init_by_array(word_t * init_key, word_t key_length, rand_ctx c)
{
   word_t i = 1, j = 0, k = NN > key_length ? NN : key_length;
   init_genrand(WORD(19650218), CTX(c));
   
   for( ; k ; --k ) 
	{
		MT(c)[i] = (MT(c)[i] ^ ((MT(c)[i - 1] ^ 
			(MT(c)[i - 1] >> 62)) * WORD(3935559000370003845))) 
         + init_key[j] + j; /* non linear */
      
      i++; j++;

      if(i >= NN) 
		{ 
			MT(c)[0] = MT(c)[NN - 1];
			i = 1; 
		}
        
		if(j >= key_length) 
			j = 0;
   }

   for (k = NN - 1 ; k ; --k) 
   {
      MT(c)[i] = (MT(c)[i] ^ ((MT(c)[i - 1] ^ 
		  (MT(c)[i - 1] >> 62)) * WORD(2862933555777941757))) 
        - i; /* non linear */
        
      i++;
        
		if(i >= NN) 
		{ 
			MT(c)[0] = MT(c)[NN - 1];
			i = 1; 
      }
   }

   MT(c)[0] = WORD(1) << 63; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number in [0, 2^64-1]-interval */

word_t mt_word(rand_ctx c)
{
   word_t i, x;

	if (IX(c) >= NN) 
	{
      if (IX(c) == NN + 1) /* if init_genrand() has not been called, */
         init_genrand(WORD(5489), (CTX(c))); /* a default initial seed is used */

      for( i = 0 ; i < NN - MM ; ++i ) 
		{
			x = (MT(c)[i] & ~LM) | (MT(c)[i + 1] & LM);
         MT(c)[i] = MT(c)[i + MM] ^ (x >> 1) 
                       ^ MG(c)[x & 1];
      }
        
      for( ; i < NN - 1 ; ++i ) 
		{
         x = (MT(c)[i] & ~LM) | (MT(c)[i + 1] & LM);
          MT(c)[i] = MT(c)[i + (MM - NN)] ^ (x >> 1) 
                        ^ MG(c)[x & 1];
      }
        
      x = (MT(c)[NN - 1] & ~LM) | (MT(c)[0] & LM);
      MT(c)[NN - 1] = MT(c)[MM - 1] ^ (x >> 1) 
                         ^ MG(c)[x & 1];

      IX(c) = 0;
   }
  
   /* Tempering */
   x = MT(c)[IX(c)++];
   x ^= (x >> 29) & WORD(0x5555555555555555);
   x ^= (x << 17) & WORD(0x71D67FFFEDA60000);
   x ^= (x << 37) & WORD(0xFFF7EEE000000000);
   x ^= (x >> 43);

   return x;
}

#endif
