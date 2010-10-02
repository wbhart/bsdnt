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

#include "nn.h"
#include "nn_quadratic_arch.h"

#ifndef HAVE_ARCH_nn_mul_classical

word_t nn_mul_classical(nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2)
{
   len_t i;
   word_t ci = 0;
  
   ci = nn_mul1(r, a, m1, b[0]); 
   
   for (i = 1; i < m2; i++)
   {
      r[m1 + i - 1] = ci;
      ci = nn_addmul1(r + i, a, m1, b[i]);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_muladd_classical

word_t nn_muladd_classical(nn_t r, nn_src_t a, nn_src_t b, 
                                     len_t m1, nn_src_t c, len_t m2)
{
   len_t i;
   word_t ci = 0;
  
   ci = nn_muladd1(r, a, b, m1, c[0]); 

   for (i = 1; i < m2; i++)
   {
      r[m1 + i - 1] = ci;
      ci = nn_addmul1(r + i, b, m1, c[i]);
   }

   return ci;
}

#endif
