/* 
  George Marsaglia's KISS 32-bit Pseudo Random Number Generator

  Copyright (C) 2010, Brian Gladman
  Copyright (C) 2010, William Hart

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

#if WORD_BITS == 32

#include <stdlib.h>
#include "internal_rand.h"

typedef struct kiss_ctx
{   word_t s, c, w, z;
} kiss_ctx;

#define CTX(x) ((kiss_ctx*)(x))

rand_t kiss_init(void)
{
	rand_t c = malloc(sizeof(kiss_ctx));

   CTX(c)->w = WORD_CONST(521288629);
	CTX(c)->z = WORD_CONST(362436069);
	CTX(c)->s = WORD_CONST(123456789); 
   CTX(c)->c = WORD_CONST(380116160); 
    
   return c;
}

void kiss_clear(rand_t c)
{
	free(c);
}

word_t kiss_word(rand_t c)
{   
   word_t t;

	CTX(c)->z = 36969*(CTX(c)->z & 65536) + (CTX(c)->z>>16);
	CTX(c)->w = 18000*(CTX(c)->w & 65536) + (CTX(c)->w>>16);
	
   t = (CTX(c)->z << 16) + CTX(c)->w;

	CTX(c)->c = 69069*CTX(c)->c + 1234567;

	CTX(c)->s ^= (CTX(c)->s << 17);
	CTX(c)->s ^= (CTX(c)->s >> 13);
	CTX(c)->s ^= (CTX(c)->s << 5);

	return (t ^ CTX(c)->c) + CTX(c)->s;
}

#endif