/* 
  Copyright (C) 2010, 2013 William Hart

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
#include "nn_subquadratic_arch.h"

#ifndef HAVE_ARCH_nn_mul_kara

void nn_mul_kara(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n)
{
   len_t m2 = m/2;
   len_t l = m - m2;
   len_t h = n - l;
   word_t ci;

   nn_t t;
   TMP_INIT;

   ASSERT(m >= n);
   ASSERT(n >= 3*m/4);
   ASSERT(p != a);
   ASSERT(p != b);
   ASSERT(n > 1);

   p[l]       = nn_add(p, a, l, a + l, m2);
   p[2*l + 1] = nn_add(p + l + 1, b, l, b + l, h);
   
   TMP_START;
   t = TMP_ALLOC(2*l + 2);
   
   nn_mul_classical(t, p + l + 1, l + 1, p, l + 1); 
   
   nn_mul_classical(p, a, l, b, l);
   nn_mul_classical(p + 2*l, a + l, m2, b + l, h);
   
   ci = -nn_sub(t, t, 2*l + 1, p, 2*l);
   t[2*l + 1] = ci - nn_sub(t, t, 2*l + 1, p + 2*l, m2 + h);
   
   nn_add(p + l, p + l, m + h, t, 2*l + 1);
  
   TMP_END;
}

#endif