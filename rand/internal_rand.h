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

#ifndef INTERNAL_RAND_H
#define INTERNAL_RAND_H

#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>

#include "bsdnt_rand.h"

/* George Marsaglia's KISS 64-bit Pseudo Random Number Generator */

typedef struct kiss_ctx
{
    uint64_t ty;
    uint64_t x, c, y, z;
} kiss_ctx;

kiss_ctx *kiss_start(void);
uint64_t kiss_rand_uint64(kiss_ctx *ctx);
void kiss_end(kiss_ctx *ctx);

/* George Marsaglia's Super KISS 64-bit Pseudo Random Number Generator */

typedef struct
{	uint64_t ty;
    uint64_t q[20632]; 
	uint64_t carry;
	uint64_t xcng;
	uint64_t xs;
	uint64_t indx;
} skiss_ctx;

skiss_ctx *skiss_start(void);
uint64_t skiss_rand_uint64(skiss_ctx *ctx);
void skiss_end(skiss_ctx *ctx);

/* Mersenne Twister */

#define NN 312

typedef struct
{	uint64_t ty;
    uint64_t mt[NN];
	uint64_t mag01[2];
	uint64_t mti;
} mt_ctx;

mt_ctx *mt_start(void);
void init_by_array64(uint64_t init_key[], uint64_t key_length, mt_ctx *ctx);
uint64_t mt_rand_uint64(mt_ctx *ctx);
void mt_end(mt_ctx *ctx);

#endif
