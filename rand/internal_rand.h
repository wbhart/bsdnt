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

#include "../helper.h"
#include "bsdnt_rand.h"

/***************************************************************************
 
   George Marsaglia's KISS Pseudo Random Number Generator

***************************************************************************/

/* 
   Initialise the KISS random state. 
   A default random seed is used.
*/
rand_t kiss_start(void);

/* 
   Clean up the KISS random state 
*/
void kiss_end(rand_t ctx);

/* 
   Generate a random word with the KISS PRNG. 
*/
word_t kiss_word(rand_t ctx);

/***************************************************************************
 
   George Marsaglia's Super KISS Pseudo Random Number Generator

***************************************************************************/

/* 
   Initialise the SKISS random state. 
   A default random seed is used.
*/
rand_t skiss_start(void);

/* 
   Clean up the SKISS random state 
*/
void skiss_end(rand_t ctx);

/* 
   Generate a random word with the SKISS PRNG. 
*/
word_t skiss_word(rand_t ctx);

/***************************************************************************
 
   Mersenne twister

***************************************************************************/

/* 
   Initialise the Mersenne twister random state. 
   A default random seed is used.
*/
rand_t mt_start(void);

/* 
   Clean up the Mersenne twister random state 
*/
void mt_end(rand_t ctx);

/* 
   Initialise the Mersenne twister with a random seed.
*/
void init_genrand(word_t seed, rand_t c);

/* 
   Initialise the Mersenne twister with a "key" of length 
   key_length instead of a random seed. 
*/
void init_by_array(word_t * init_key, word_t key_length, rand_t ctx);

/* 
   Generate a random word with the Mersenne twister. 
*/
word_t mt_word(rand_t ctx);

#endif
