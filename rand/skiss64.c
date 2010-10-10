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

#include "internal_rand.h"

typedef struct
{	uint64_t q[20632]; 
	uint64_t carry;
	uint64_t xcng;
	uint64_t xs;
	uint64_t indx;
} skiss_ctx;

#define CTX(x) ((skiss_ctx*)(x))

#define CNG(x)  ( (x)->xcng = 6906969069ull * (x)->xcng + 123 )
#define XS(x)   ( (x)->xs ^= (x)->xs << 13, (x)->xs ^= (x)->xs >> 17, (x)->xs ^= (x)->xs << 43 )
#define SUPR(x) ( (x)->indx < 20632 ? (x)->q[(x)->indx++] : refill((x)) )
#define KISS(x) ( SUPR(x) + CNG(x) + XS(x) )

uint64_t refill(rand_t c)
{
    uint64_t i, z, h;

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

rand_t skiss_start(void)
{   uint64_t i;
	rand_t c = malloc(sizeof(skiss_ctx));
    
    CTX(c)->carry = 36243678541ull;
	CTX(c)->xcng = 12367890123456ull;
	CTX(c)->xs = 521288629546311ull;
	CTX(c)->indx = 20632ull;

    for( i = 0 ; i < 20632 ; ++i ) 
		CTX(c)->q[i] = CNG(CTX(c)) + XS(CTX(c));

    return c; 
}

void skiss_end(rand_t ctx)
{
	free(ctx);
}

uint64_t skiss_uint64(rand_t c)
{
    return  SUPR(CTX(c)) + CNG(CTX(c)) + XS(CTX(c));
}
