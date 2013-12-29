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

#include <string.h>
#include "helper.h"
#include "internal_rand.h"

static const char *name[3] = { "Kiss", "Mersenne_Twister", "Super_Kiss" };

rand_t set_rand_algorithm(random_algorithm a)
{   
   rand_t r;

   switch(a)
   {
   case KISS:
      r.init = (rand_init_f) kiss_init;
      r.clear = (rand_clear_f) kiss_clear;
      r.word = (rand_word_f) kiss_word;
      strcpy(r.name, name[0]);
      break;
   case MERSENNE_TWISTER:
      r.init = (rand_init_f) mt_init;
      r.clear = (rand_clear_f) mt_clear;
      r.word = (rand_word_f) mt_word;
      strcpy(r.name, name[1]);
      break;
   case SUPER_KISS:
      r.init = (rand_init_f) skiss_init;
      r.clear = (rand_clear_f) skiss_clear;
      r.word = (rand_word_f) skiss_word;
      strcpy(r.name, name[2]);
      break;
   default:
      r.init = (rand_init_f) kiss_init;
      r.clear = (rand_clear_f) kiss_clear;
      r.word = (rand_word_f) kiss_word;
      strcpy(r.name, name[0]);
      break;
   }
   
   r.ctx = NULL;

   return r;
}
