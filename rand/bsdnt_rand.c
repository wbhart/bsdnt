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

#include "internal_rand.h"

rand_t rand_start(random_algorithm a)
{   rand_t ctx;

    switch(a)
    {
    case KISS:
        ctx = kiss_start();
        *((uint64_t*)ctx) = (uint64_t)KISS;
        break;
    case SUPER_KISS:
        ctx = skiss_start();
        *((uint64_t*)ctx) = (uint64_t)SUPER_KISS;
        break;
    case MERSENNE_TWISTER:
        ctx = mt_start();
        *((uint64_t*)ctx) = (uint64_t)MERSENNE_TWISTER;
        break;
    default:
        ctx = 0;
    }
    return ctx;
}

void rand_end(rand_t ctx)
{
    switch((random_algorithm)*((uint64_t*)ctx))
    {
    case KISS:
        kiss_end((kiss_ctx*)ctx);
        break;
    case SUPER_KISS:
        skiss_end((skiss_ctx*)ctx);
        break;
    case MERSENNE_TWISTER:
        mt_end((mt_ctx*)ctx);
        break;
    default:
        ;
    }
}

uint64_t rand_uint64(rand_t ctx)
{
    switch((random_algorithm)*((uint64_t*)ctx))
    {
    case KISS:
        return kiss_rand_uint64((kiss_ctx*)ctx);
    case SUPER_KISS:
        return skiss_rand_uint64((skiss_ctx*)ctx);
    case MERSENNE_TWISTER:
        return mt_rand_uint64((mt_ctx*)ctx);
    default:
        abort();
    }
}
