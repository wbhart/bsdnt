/* 
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

#ifndef BSDNT_GENERIC_H
#define BSDNT_GENERIC_H

#include "nn.h"

typedef enum
{
   NN 
} type_t;

typedef enum
{
   ANY, NONZERO, ODD, FULL
} flag_t;

typedef struct node_t
{
   type_t type;
   void * ptr;
   struct node_t * next;
} node_t;

/*
   Send the given error message to stderr.
*/
void talker(const char * str);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag.

   randoms(NONZERO, state, &w1, &w2, NULL);
*/
void randoms(flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag and
   which are below the given limit.

   randoms_upto(100, ANY, state, &w1, &w2, NULL);
*/
void randoms_upto(word_t limit, flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to nn_t's, generate 
   random integers which have the property specified by the flag and
   which have the specified number of limbs. These can only be
   cleaned up by calling gc_cleanup() (see below).

   randoms_upto(100, FULL, state, &a, &b, NULL);
*/
void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

/*
   Clean up any objects allocated by the random functions above.
   Note *all* objects are cleaned up. We have no way of knowing 
   which are still in use, so it is assumed none of them are still
   in use.
*/
void gc_cleanup(void);

#endif

