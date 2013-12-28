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

#include "zz.h"

void zz_init(zz_ptr r)
{
   r->n = NULL;
   r->size = 0;
   r->alloc = 0;
}

void zz_init_fit(zz_ptr r, len_t m)
{
   r->n = (nn_t) malloc(sizeof(word_t)*m);
   r->alloc = m;
   r->size = 0;
}

void zz_clear(zz_ptr r)
{
   free(r->n);
}

void zz_fit(zz_ptr r, len_t m)
{
   if (r->alloc < m) 
   {
      r->n = (nn_t) realloc(r->n, sizeof(word_t)*m);
      r->alloc = m;
   }
}

void zz_normalise(zz_ptr r)
{
   if (r->size < 0)
      r->size = -nn_normalise(r->n, -r->size);
   else
      r->size = nn_normalise(r->n, r->size);
}

int zz_equali(zz_srcptr r, sword_t c)
{
   if (c == 0)
      return r->size == 0;
   else if (c > 0)
      return r->size == 1 && r->n[0] == c;
   else
      return r->size == -1 && r->n[0] == -c;
}

int zz_cmpi(zz_srcptr a, sword_t b)
{
   len_t asize = a->size;
   len_t bsize = b == 0 ? 0 : (b > 0 ? 1 : -1);
   word_t abs = BSDNT_ABS(b);
   
   if (asize != bsize)
      return asize - bsize;

   else if (asize == 0)
      return 0;

   if (a->n[0] == abs)
      return 0;
   
   if (a->n[0] > abs)
      return asize;
   else
      return -asize;
}

int zz_cmp(zz_srcptr a, zz_srcptr b)
{
   len_t asize = a->size;
   len_t bsize = b->size;
   int sgn;

   if (asize != bsize)
      return asize - bsize;

   sgn = nn_cmp_m(a->n, b->n, BSDNT_ABS(asize));

   return asize < 0 ? -sgn : sgn;
}

int zz_cmpabs(zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   
   if (asize != bsize)
      return asize - bsize;

   return nn_cmp_m(a->n, b->n, asize);
}

int zz_is_zero(zz_srcptr r)
{
   return r->size == 0;
}

void zz_random(zz_ptr a, rand_t state, bits_t bits)
{
   bits_t ubits = BSDNT_ABS(bits);
   len_t size = (ubits + WORD_BITS - 1)/WORD_BITS;

   zz_fit(a, size);

   nn_random(a->n, state, ubits);

   size = nn_normalise(a->n, size);
   a->size = (bits < 0 && randint(2, state) == 0) ? -size : size;
}

void zz_seti(zz_ptr r, sword_t c)
{
   if (c != 0) 
   {
      zz_fit(r, 1);
      r->n[0] = BSDNT_ABS(c);
      r->size = c > 0 ? 1 : -1;
   } else
      r->size = 0;
}

void zz_set(zz_ptr a, zz_srcptr b)
{
   len_t usize = BSDNT_ABS(b->size);
   
   if (a != b) 
   {
      zz_fit(a, usize);
      nn_copy(a->n, b->n, usize);
      a->size = b->size;
   }
}

void zz_neg(zz_ptr r, zz_srcptr a)
{
   if (r != a) 
   {
      len_t usize = BSDNT_ABS(a->size);
      zz_fit(r, usize);
      
      nn_copy(r->n, a->n, usize);
   }

   r->size = -a->size;
}

void zz_swap(zz_ptr a, zz_ptr b)
{
   NN_SWAP(a->n, b->n);
   BSDNT_SWAP(a->size, b->size);
   BSDNT_SWAP(a->alloc, b->alloc);
}

void zz_zero(zz_ptr a)
{
   a->size = 0;
}

void zz_addi(zz_ptr r, zz_srcptr a, sword_t c)
{
   if (c < 0)
      zz_subi(r, a, -c);
   else 
   {
      len_t usize = BSDNT_ABS(a->size);

      zz_fit(r, usize + 1);
   
      if (a->size >= 0) 
      {
         r->n[usize] = nn_add1(r->n, a->n, usize, c);
         r->size = usize + (r->n[usize] != 0);
      } else if (usize == 1) 
      {
         word_t d = a->n[0];

         if (d == c)
            r->size = 0;
         else
         {
            r->n[0] = d > c ? d - c : c - d;
            r->size = d > c ? -1 : 1;
         } 
      } else 
      {
         nn_sub1(r->n, a->n, usize, c);
         r->size = -usize + (r->n[usize - 1] == 0);
      }
   }
}

void zz_subi(zz_ptr r, zz_srcptr a, sword_t c)
{
   if (c < 0)
      zz_addi(r, a, -c);
   else
   {
      long usize = BSDNT_ABS(a->size);

      zz_fit(r, usize + 1);
   
      if (a->size == 0) 
      {
         r->size = -1;
         r->n[0] = c;
      } else 
      {
         if (a->size < 0) 
         {
            r->n[usize] = nn_add1(r->n, a->n, usize, c);
            r->size = -usize - (r->n[usize] != 0);
         } else if (usize == 1) 
         {
            word_t d = a->n[0];

            if (d == c)
               r->size = 0;
            else
            {
               r->n[0] = d > c ? d - c : c - d;
               r->size = d > c ? 1 : -1;
            }
         } else 
         {
            nn_sub1(r->n, a->n, usize, c);
            r->size = usize - (r->n[usize - 1] == 0);
         }
      }
   }
}

void zz_add(zz_ptr r, zz_srcptr a, zz_srcptr b)
{
   long asize = BSDNT_ABS(a->size);
   long bsize = BSDNT_ABS(b->size);
   long rsize;

   ZZ_ORDER(a, asize, b, bsize);
   
   zz_fit(r, asize + 1);

   if ((a->size ^ b->size) < 0) 
   {
      word_t bi = nn_sub(r->n, a->n, asize, b->n, bsize);
      rsize = a->size;
      
      if (bi != 0) 
      {
         nn_neg(r->n, r->n, asize);
         rsize = -rsize;
      }
   } else 
   {
      r->n[asize] = nn_add(r->n, a->n, asize, b->n, bsize);
      rsize = asize + 1;
      if (a->size < 0) rsize = -rsize;
   }

   r->size = rsize;
   zz_normalise(r);
}

void zz_sub(zz_ptr r, zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   len_t rsize;
   int sign = asize - bsize;

   ZZ_ORDER(a, asize, b, bsize);
   
   zz_fit(r, asize + 1);

   if ((a->size ^ b->size) >= 0) 
   {
      word_t bi = nn_sub(r->n, a->n, asize, b->n, bsize);
      rsize = a->size;
      
      if (bi != 0) 
      {
         nn_neg(r->n, r->n, asize);
         rsize = -rsize;
      }
   } else 
   {
      r->n[asize] = nn_add(r->n, a->n, asize, b->n, bsize);
      rsize = asize + 1;
      if (a->size < 0) rsize = -rsize;
   }

   r->size = rsize;
   if (sign < 0) 
      rsize = -rsize;
   
   zz_normalise(r);
}

void zz_mul_2exp(zz_ptr r, zz_srcptr a, bits_t exp)
{
   bits_t bits = exp % WORD_BITS;
   len_t words = exp / WORD_BITS;
   len_t usize = BSDNT_ABS(a->size);
   len_t rsize = usize + words;
   
   zz_fit(r, rsize + (bits != 0));

   if (bits == 0) 
   {
      nn_copyd(r->n + words, a->n, usize);
      rsize = usize + words;
   } else 
   {
      r->n[rsize] = nn_shl(r->n + words, a->n, usize, bits);
      rsize += (r->n[rsize] != 0);  
   }

   r->size = a->size >= 0 ? rsize : -rsize;
}

void zz_div_2exp(zz_ptr r, zz_srcptr a, bits_t exp)
{
   bits_t bits = exp % WORD_BITS;
   len_t words = exp / WORD_BITS;
   len_t usize = BSDNT_ABS(a->size);
   len_t rsize = usize - words;
   
   if (rsize <= 0)
      r->size = 0;
   else 
   {
      zz_fit(r, rsize);

      if (bits == 0)
         nn_copy(r->n, a->n + words, rsize);
      else 
      {
         nn_shr(r->n, a->n + words, rsize, bits);
         rsize -= (r->n[rsize - 1] == 0);  
      }

      r->size = a->size >= 0 ? rsize : -rsize;
   }
}

void zz_muli(zz_ptr r, zz_srcptr a, sword_t c)
{
   len_t usize = BSDNT_ABS(a->size);
   
   if (c == 0 || usize == 0)
      r->size = 0;
   else
   {
      zz_fit(r, usize + 1);

      r->n[usize] = nn_mul1(r->n, a->n, usize, BSDNT_ABS(c));
      usize += (r->n[usize] != 0);

      r->size = (c ^ a->size) >= 0 ? usize : -usize;
   }
}

sword_t zz_divremi(zz_ptr q, zz_srcptr a, sword_t b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t r, qsize = asize;
   word_t norm, binv, babs, ci;
   zz_t t;

   if (asize == 0) 
   {
      q->size = 0;
      r = b;
   } else 
   {
      zz_init_fit(t, asize);
      zz_set(t, a);

      zz_fit(q, qsize);
   
      babs = BSDNT_ABS(b);
      norm = high_zero_bits(babs);
      binv = precompute_inverse1(babs << norm);
      ci = nn_shl(t->n, t->n, asize, norm);
      r = nn_divrem1_preinv_c(q->n, t->n, asize, babs << norm, binv, ci) >> norm;
         
      qsize -= q->n[qsize - 1] == 0;
      
      q->size = (a->size ^ b) >= 0 ? qsize : -qsize;
      r = a->size >= 0 ? r : -r;

      if (q->size < 0 && r != 0) 
      {
         zz_subi(q, q, 1);
         r += b;
      }

      zz_clear(t);
   }

   return r;
}

void zz_mul(zz_ptr r, zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   len_t rsize = asize + bsize;
   
   if (asize == 0 || bsize == 0)
      rsize = 0;
   else 
   {
      zz_fit(r, rsize);

      ZZ_ORDER(a, asize, b, bsize);

      nn_mul(r->n, a->n, asize, b->n, bsize);
      rsize -= (r->n[rsize - 1] == 0);

      r->size = (a->size ^ b->size) < 0 ? -rsize : rsize;
   }
}

void zz_divrem(zz_ptr q, zz_ptr r, zz_srcptr a, zz_srcptr b)
{
   long asize = BSDNT_ABS(a->size);
   long bsize = BSDNT_ABS(b->size);
   long rsize = bsize;
   long qsize = asize - bsize + 1;
   
   zz_set(r, a);

   if (asize < bsize)
      q->size = 0;
   else 
   {
      zz_fit(q, qsize);
   
      nn_divrem(q->n, r->n, asize, b->n, bsize);
         
      qsize -= q->n[qsize - 1] == 0;
      rsize = nn_normalise(r->n, rsize);

      q->size = (a->size ^ b->size) >= 0 ? qsize : -qsize;
      r->size = a->size >= 0 ? rsize : -rsize;

      if (q->size < 0 && r->size != 0) 
      {
         zz_subi(q, q, 1);
         zz_add(r, r, b);
      }
   }
}

void zz_div(zz_ptr q, zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   len_t rsize = bsize;
   len_t qsize = asize - bsize + 1;
   len_t qsign = (a->size ^ b->size);
   zz_t r;

   if (asize < bsize)
      q->size = 0;
   else {
      zz_init(r);
      zz_set(r, a);

      zz_fit(q, qsize);
   
      if (qsign < 0) /* TODO: test one remainder limb first */
      {
         nn_divrem(q->n, r->n, asize, b->n, bsize);
         rsize = nn_normalise(r->n, rsize);
      } else
         nn_div(q->n, r->n, asize, b->n, bsize);

      qsize -= q->n[qsize - 1] == 0;
      q->size = qsign >= 0 ? qsize : -qsize;
      
      if (q->size < 0 && rsize != 0)
         zz_subi(q, q, 1);

      zz_clear(r);
   }
}

void zz_gcd(zz_ptr g, zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   len_t size;
   nn_t ta, tb;
   TMP_INIT;

   if (asize == 0)
      zz_set(g, b);
   else if (bsize == 0)
      zz_set(g, a);
   else 
   {
      TMP_START;

      ZZ_ORDER(a, asize, b, bsize);

      ta = TMP_ALLOC(asize);
      tb = TMP_ALLOC(bsize);

      nn_copy(ta, a->n, asize);
      nn_copy(tb, b->n, bsize);

      zz_fit(g, bsize);

      size = nn_gcd(g->n, ta, asize, tb, bsize);
      
      g->size = (a->size & b->size) < 0 ? -size : size;

      TMP_END;
   }
}

void zz_xgcd(zz_ptr g, zz_ptr s, zz_ptr t, zz_srcptr a, zz_srcptr b)
{
   len_t asize = BSDNT_ABS(a->size);
   len_t bsize = BSDNT_ABS(b->size);
   len_t gsize, tsize;
   nn_t ta, tb;
   TMP_INIT;

   if (asize == 0)
   {
      zz_set(g, b);
      zz_seti(s, 0);
      zz_seti(t, 1);
   }
   else if (bsize == 0)
   {
      zz_set(g, a);
      zz_seti(s, 1);
      zz_seti(t, 0);
   } else 
   {
      TMP_START;

      ZZ_ORDER(a, asize, b, bsize);

      ta = TMP_ALLOC(asize);
      tb = TMP_ALLOC(bsize);

      nn_copy(ta, a->n, asize);
      nn_copy(tb, b->n, bsize);

      zz_fit(g, bsize);
      zz_fit(s, asize);
      zz_fit(t, asize);

      gsize = nn_xgcd(g->n, t->n, ta, asize, tb, bsize);
      
      gsize = (a->size & b->size) < 0 ? -gsize : gsize;
      tsize = nn_normalise(t->n, asize);

      t->size = b->size < 0 && a->size > 0 ? tsize : -tsize;
      g->size = gsize;

      zz_mul(s, b, t); /* compute other cofactor s = (g - bt)/a */
      zz_sub(s, g, s);
      zz_div(s, s, a); /* TODO: use divexact */

      TMP_END;
   }
}

char * zz_get_str(zz_srcptr a)
{
   len_t size = BSDNT_ABS(a->size);
   char * str;
   nn_t t;
   TMP_INIT;
   
   TMP_START;
   
   t = TMP_ALLOC(size);
  
   nn_copy(t, a->n, size);

   str = nn_get_str(t, size);

   TMP_END;

   return str;
}

size_t zz_set_str(zz_t a, const char * str)
{
   int sgn = 0;
   len_t size;
   size_t digits;
   
   if (str[0] == '-') 
   {
      sgn = 1;
      str++;
   }

   digits = strspn(str, "0123456789");
   
   /* 0.0519... is log_{2^64}(10) */
   size = (len_t) ceil(0.0519051265 * digits * (64/WORD_BITS));
   zz_fit(a, size);

   digits = nn_set_str(a->n, &size, str);

   a->size = sgn ? -size : size;

   return digits + sgn;
}

void zz_print(zz_srcptr a)
{
   char * str = zz_get_str(a);

   printf("%s", str);

   free(str);
}