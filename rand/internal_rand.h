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

extern rand_t kiss_start(void);
extern uint64_t kiss_uint64(rand_t ctx);
extern void kiss_end(rand_t ctx);

/* George Marsaglia's Super KISS 64-bit Pseudo Random Number Generator */

extern rand_t skiss_start(void);
extern uint64_t skiss_uint64(rand_t ctx);
extern void skiss_end(rand_t ctx);

/***************************************************************************
 
   Mersenne twister

***************************************************************************/

/* 
   Initialise the Mersenne twister random state. 
   A default random seed is used.
*/
extern rand_t mt_start(void);

/* 
   Clean up the Mersenne twister random state 
*/
extern void mt_end(rand_t ctx);

/* 
   Initialise the Mersenne twister with a random seed.
*/
void init_genrand(word_t seed, rand_t c)

/* 
   Initialise the Mersenne twister with a "key" of length 
   key_length instead of a random seed. 
*/
extern void init_by_array(word_t init_key[], word_t key_length, rand_t ctx);

/* 
   Generate a random word with the Mersenne twister. 
*/
extern word_t mt_word(rand_t ctx);

#endif
