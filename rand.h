/* 
  Copyright (C) 2010, William Hart
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

#ifndef BSDNT_RAND_H
#define BSDNT_RAND_H

#include "helper.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* BSDNT Interface */

typedef void * rand_ctx;

typedef rand_ctx (* rand_init_f)(void);
typedef void (* rand_clear_f)(rand_ctx);
typedef word_t (* rand_word_f)(rand_ctx);

typedef struct rand_t
{
    rand_init_f init;
    rand_clear_f clear;
    rand_word_f word;
    rand_ctx ctx;
    char name[24];
} rand_t;

typedef enum { RAND_START = 0, KISS, MERSENNE_TWISTER, SUPER_KISS, RAND_END } random_algorithm;

rand_t set_rand_algorithm(random_algorithm a);

static inline
void randinit(rand_t * state)
{   
    *state = set_rand_algorithm(KISS);
    state->ctx = state->init();
}

static inline
void randclear(rand_t state)
{
    state.clear(state.ctx);
}

static inline
word_t randword(rand_t state) 
{
	return state.word(state.ctx);
}

static inline
word_t randint(word_t m, rand_t state)
{
   return ((word_t) state.word(state.ctx) % m);
}

#ifdef __cplusplus
 }
#endif

#endif
