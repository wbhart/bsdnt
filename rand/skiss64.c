/* 
  George Marsaglia's SUPRKISS64, period 5 * 2^1320480 * (2^64 - 1)

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

#include "../helper.h"

#if WORD_BITS == 64

#include <stdlib.h>
#include "internal_rand.h"

typedef struct
{	
   word_t q[20632]; 
	word_t carry;
	word_t xcng;
	word_t xs;
	word_t indx;
} skiss_ctx;

#define CTX(x) ((skiss_ctx *)(x))

#define CNG(x)  ( (x)->xcng = WORD(6906969069) * (x)->xcng + 123 )
#define XS(x)   ( (x)->xs ^= (x)->xs << 13, (x)->xs ^= (x)->xs >> 17, (x)->xs ^= (x)->xs << 43 )
#define SUPR(x) ( (x)->indx < 20632 ? (x)->q[(x)->indx++] : refill((x)) )
#define KISS(x) ( SUPR(x) + CNG(x) + XS(x) )

word_t refill(rand_ctx c)
{
   word_t i, z, h;

   for( i = 0 ; i < 20632 ; ++i)
   { 
      h = CTX(c)->carry & 1;
      z = ((CTX(c)->q[i] << 41) >> 1) + ((CTX(c)->q[i] << 39) >> 1) 
                  + (CTX(c)->carry >> 1);
      CTX(c)->carry = (CTX(c)->q[i] >> 23) + (CTX(c)->q[i] >> 25) + (z >> 63);
      CTX(c)->q[i] = ~((z << 1) + h); 
   }

   CTX(c)->indx = 1; 

   return CTX(c)->q[0];
}

rand_ctx skiss_init(void)
{   
   word_t i;
	rand_ctx c = malloc(sizeof(skiss_ctx));
    
   CTX(c)->carry = WORD(36243678541);
	CTX(c)->xcng = WORD(12367890123456);
	CTX(c)->xs = WORD(521288629546311);
	CTX(c)->indx = WORD(20632);

   for (i = 0 ; i < 20632 ; ++i) 
	   CTX(c)->q[i] = CNG(CTX(c)) + XS(CTX(c));

   return c; 
}

void skiss_clear(rand_ctx c)
{
	free(c);
}

word_t skiss_word(rand_ctx c)
{
   return  SUPR(CTX(c)) + CNG(CTX(c)) + XS(CTX(c));
}

#endif
