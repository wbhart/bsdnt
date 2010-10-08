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

#define CNG(x)  ( x->xcng = 6906969069ull * x->xcng + 123 )
#define XS(x)   ( x->xs ^= x->xs << 13, x->xs ^= x->xs >> 17, x->xs ^= x->xs << 43 )
#define SUPR(x) ( x->indx < 20632 ? x->q[x->indx++] : refill(x) )
#define KISS(x) ( SUPR(x) + CNG(x) + XS(x) )

uint64_t refill(skiss_ctx *ctx)
{
    uint64_t i, z, h;

    for( i = 0 ; i < 20632 ; ++i)
    { 
        h = ctx->carry & 1;
        z = ((ctx->q[i] << 41) >> 1) + ((ctx->q[i] << 39) >> 1) 
                    + (ctx->carry >> 1);
        ctx->carry = (ctx->q[i] >> 23) + (ctx->q[i] >> 25) + (z >> 63);
        ctx->q[i] = ~((z << 1) + h); 
    }
    ctx->indx = 1; 
    return ctx->q[0];
}

skiss_ctx *skiss_start(void)
{   uint64_t i;
	skiss_ctx *ctx = (skiss_ctx*)malloc(sizeof(skiss_ctx));
    
    ctx->carry = 36243678541ull;
	ctx->xcng = 12367890123456ull;
	ctx->xs = 521288629546311ull;
	ctx->indx = 20632ull;

    for( i = 0 ; i < 20632 ; ++i ) 
		ctx->q[i] = CNG(ctx) + XS(ctx);

    return ctx; 
}

void skiss_end(skiss_ctx *ctx)
{
	free(ctx);
}

uint64_t skiss_rand_uint64(skiss_ctx *ctx)
{
    return  SUPR(ctx) + CNG(ctx) + XS(ctx);
}
