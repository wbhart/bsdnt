/* 
  George Marsaglia's KISS 64-bit Pseudo Random Number Generator

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

kiss_ctx *kiss_start(void)
{
	kiss_ctx *ctx = (kiss_ctx*)malloc(sizeof(kiss_ctx));
    ctx->x = 1234567890987654321ull;
	ctx->c = 123456123456123456ull; 
    ctx->y = 362436362436362436ull;
	ctx->z = 1066149217761810ull;
    return ctx;
}

void kiss_end(kiss_ctx *ctx)
{
	free(ctx);
}

uint64_t kiss_rand_uint64(kiss_ctx *ctx)
{   uint64_t t;

	t = (ctx->x << 58) + ctx->c;
	ctx->c = ctx->x >> 6;
	ctx->x += t;
	ctx->c += (ctx->x < t);

	ctx->y ^= (ctx->y << 13);
	ctx->y ^= (ctx->y >> 17);
	ctx->y ^= (ctx->y << 43);

    ctx->z = 6906969069UL * ctx->z + 1234567;

	return (uint64_t)( ctx->x + ctx->y + ctx->z );
}
