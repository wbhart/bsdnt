/* 
  George Marsaglia's SUPRKISS32, period 5 * 2^1320481 * (2^32 - 1)

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

#if WORD_BITS == 32

#include <stdlib.h>
#include "internal_rand.h"

typedef void * rand_ctx;

typedef struct
{   
   word_t q[41265];
	word_t carry;
	word_t xcng;
	word_t xs;
	word_t indx;
} skiss_ctx;

#define CTX(c) ((skiss_ctx *)(c))

#define CNG(p)  (p->xcng = 69069UL * p->xcng + 123ul)
#define XS(p)   (p->xs ^= p->xs << 13, p->xs ^= p->xs >> 17, \
                  p->xs ^= p->xs << 5)
#define SUPR(p) ((p->indx < 41265L) ? p->q[p->indx++] : refill(p))
#define KISS(p) (SUPR(p) + CNG(p) + XS(p))

rand_ctx skiss_init(void)
{   
   long i;
	rand_t c = malloc(sizeof(skiss_ctx));

   CTX(c)->carry = 362UL;
	CTX(c)->xcng = 1236789UL;
	CTX(c)->xs = 521288629UL;
	CTX(c)->indx = 41265UL;

   for (i = 0; i < 41265; ++i)
      CTX(c)->q[i] = (CNG(CTX(c)) + XS(CTX(c)));

   return c;
}

void skiss_clear(rand_t c)
{
	free(c);
}

word_t refill(rand_t c)
{   
   long i;
   word_t z, h;

   for (i = 0; i < 41265; ++i)
   {
      h = CTX(c)->carry & 1UL;
      z = ((CTX(c)->q[i] << 9) >> 1) + ((CTX(c)->q[i] << 7) >> 1) 
         + (CTX(c)->carry >> 1);
      CTX(c)->carry = (CTX(c)->q[i] >> 23) + (CTX(c)->q[i] >> 25) 
         + (z >> 31);
      CTX(c)->q[i] = ~((z << 1) + h);
   }

   CTX(c)->indx = 1UL;
    
   return CTX(c)->q[0];
}

word_t skiss_word(rand_t c)
{
   return  KISS(CTX(c));
}

#endif
