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

#ifndef BSDNT_NN_H
#define BSDNT_NN_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#if defined(__MINGW32__)
	#include <malloc.h>
#endif

#include "helper.h"
#include "rand.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define NN_SWAP(a, b) \
   do {               \
      nn_t __t = a;   \
      a = b;          \
      b = __t;        \
   } while (0)

/**********************************************************************
 
    Random functions

**********************************************************************/

/*
   Set the first m words of a to random words.
*/
void nn_random(nn_t a, rand_t state, len_t m);

/*
   Randomly set bits of a to 1 in a random, sparse manner. 
*/
void nn_test_random1(nn_t a, rand_t state, len_t m);

/*
   Set a to a number with long strings of 1's and 0's in its
   binary representation.
*/
void nn_test_random2(nn_t a, rand_t state, len_t m);

/*
   Randomly select one of nn_random, nn_test_random1 and
   nn_test_random2 and set a to the result.
*/
void nn_test_random(nn_t a, rand_t state, len_t m);

/**********************************************************************
 
    Memory management

**********************************************************************/

/*
   Allocates an array of m words and returns it. Despite the
   name, the words are not initialised to zero.
*/
static inline
nn_t nn_init(len_t m)
{
   if (m) return (nn_t) malloc(m*sizeof(word_t));
   else return NULL;
}

/*
   Free the memory used by the nn_t a.
*/

static inline
void nn_clear(nn_t a)
{
   free(a);
}

/**********************************************************************
 
    I/O functions

**********************************************************************/

/*
   Print the nn_t {a, m} in hexadecimal, with a space between each
   word. If m is zero, only 0 is printed.
*/
void nn_printx(nn_src_t a, len_t m);

/*
   Print the nn_t {a, m} in hexadecimal, with a space between each
   word, but only printing at worst, the first two words of a, an 
   ellipsis (if m > 4) and the final two words of a. If m is zero, 
   only 0 is printed.
*/
void nn_printx_short(nn_src_t a, len_t m);

/*
   Print information about which words of a and b differ and actually
   print the first and last words which differ, for both bignums. 
   This function is used for diagnostic purposes in test code. If 
   there is no difference, "don't differ" is printed.
*/
void nn_printx_diff(nn_src_t a, nn_src_t b, len_t m);

/*
   Return a string representation of {a, m} in decimal. The user is
   responsible for freeing the string.
*/
char * nn_get_str(nn_src_t a, len_t m);

/*
   Set {a, *len} to the natural number given by the string str. If
   the string is "0" then len is set to 0. The function returns the
   number of digits read, where "0" counts as a single digit.
*/
size_t nn_set_str(nn_t a, len_t * len, const char * str);

/*
   Convert {a, len} to the given base. Digits are output into str, which must
   be long enough to store the result, i.e. floor(log_[base]({a, len})+1).
   The output is not ASCII digits, and is big-endian. For example, the nn_t
   value 0x12340078 becomes [0x12, 0x34, 0x00, 0x78]. The return value is the
   number of output digits. base must be from 2 to 256 inclusive. {a, len} is
   destroyed in the process.
*/
size_t nn_get_str_raw(unsigned char *str, int base, nn_t a, len_t len);

/*
   Read str (of length size) as a big-endian number in the given base, and
   output into a. a must be large enough to store the result. The return
   value is the number of words in a that were written to. This is the
   inverse of nn_get_str_raw.
*/
len_t nn_set_str_raw(nn_t a, const unsigned char * str, size_t size, int base);


/*
   Print {a, m} in decimal to stdout. If m == 0 then 0 is printed.
*/
void nn_print(nn_src_t a, len_t m);

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

/*
   Copy the m words at b to a. Aliasing is allowed, but is 
   not currently dealt with specially, i.e. the data is 
   actually copied. Words are copied in order of increasing
   index.
*/
static inline
void nn_copy(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = b[i];
}

/*
   Copy the m words at b to a. Aliasing is allowed, but is 
   not currently dealt with specially, i.e. the data is 
   actually copied. Words are copied in order of decreasing
   index.
*/
static inline
void nn_copyd(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = m - 1; i >= 0; i--)
      a[i] = b[i];
}

/*
   Set the m words at b to 0.
*/
static inline
void nn_zero(nn_t a, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = 0;
}

/*
   The pair {a, m} is said to be normalised if either m is
   zero, or a[m-1] is non-zero. In other words, {a, m} has
   no leading zero words. This function normalises {a, m}
   by returning the largest value m0 <= m for which {a, m0}
   is normalised.
*/
static inline
len_t nn_normalise(nn_t a, len_t m)
{
   while ((m != 0) && (a[m - 1] == 0))
      m--;

   return m;
}

/**********************************************************************
 
    Logical bit manipulation functions

**********************************************************************/

/* 
   Set bit b of the bignum a to 1, where bits are counted from the
   least significant bit, starting with bit 0. The given bit must
   lie within the allocated range for a.
*/
void nn_bit_set(nn_t a, bits_t b);

/* 
   Clear bit b of the bignum a to 0, where bits are counted from the
   least significant bit, starting with bit 0. The given bit must
   lie within the allocated range for a.
*/
void nn_bit_clear(nn_t a, bits_t b);

/* 
   Return 1 if the bit b of the bignum a is 1, otherwise return 0.
   Bits are counted from the least significant bit, starting with bit 
   0. The specified bit must lie within the allocated range for a.
*/
int nn_bit_test(nn_src_t a, bits_t b);

/**********************************************************************
 
    Linear arithmetic functions

**********************************************************************/

/*
   Set a to the twos complement negation of b, where b is m words 
   in length. Return any borrow. The word ci is a carry-in. It is
   effectively subtracted from the result. The carry-in can be at
   most 1.
*/
word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci);

/*
   Set a to the twos complement negation of b, where b is m words
   in length. Return any borrow.
*/
#define nn_neg(a, b, m) \
    nn_neg_c(a, b, m, 0)

/*
   Set a = b + c where b is m words in length, and c is a word. 
   Return any carry out. 
*/
word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b - c where b is m words in length, and c is a word. 
   Return any borrow out. 
*/
word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b + c + ci where b and c are both m words in length,
   ci is a "carry in". Return any carry out. The carry-in may be
   at most 1.
*/
word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci);

/*
   Set a = b + c where b and c are both m words in length. Return 
   any carry out. 
*/
#define nn_add_m(a, b, c, m) \
   nn_add_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length,
   bi is a "borrow". Return any borrow. The borrow-in may be at
   most 1.
*/
word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi);

/*
   Set a = b - c where b and c are both m words in length. Return 
   any borrow. 
*/
#define nn_sub_m(a, b, c, m) \
   nn_sub_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "carry in". We return the carry out. The carry-in 
   may be at most 1.
*/
static inline
word_t nn_add_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ASSERT(bm >= cm);
   ci = nn_add_mc(a, b, c, cm, ci);
   return nn_add1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   and bm >= cm. We return the carry out. 
*/
#define nn_add(a, b, bm, c, cm) \
   nn_add_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "borrow in". We return any borrow out. The
   borrow-in may be at most 1.
*/
static inline
word_t nn_sub_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ASSERT(bm >= cm);
   ci = nn_sub_mc(a, b, c, cm, ci);
   return nn_sub1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   and bm >= cm. We return any borrow out. 
*/
#define nn_sub(a, b, bm, c, cm) \
   nn_sub_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length,
   ci is a "carry in". Return any carry out. Assumes 0 <= bits
   < WORD_BITS. Assumes ci is not more than the given number of 
   bits.
*/
word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b << bits) where b is m words in length. Return 
   any carry out. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shl(a, b, m, bits) \
   nn_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, ci is a "carry in". Return any carry out from the low
   end. Assumes 0 <= bits < WORD_BITS. Assumes at most the top 
   "bits" bits of ci are nonzero.
*/
word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b >> bits) where b is m words in length. Return 
   any carry out from the low end. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shr(a, b, m, bits) \
   nn_shr_c(a, b, m, bits, (word_t) 0)

/*
   Set a = b * c + ci where b is m words in length, c is a word and
   ci is a "carry in". Return any carry out. 
*/
word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = b * c where b is m words in length and c is a word. 
   Return any carry out.
*/
#define nn_mul1(a, b, m, c) \
   nn_mul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is returned. 
*/
word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a + b * c where a and b are m words in length, and c 
   is a word. The carry out is returned. 
*/
#define nn_addmul1(a, b, m, c) \
   nn_addmul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a - b * c - ci where a and b are m words in length, c 
   is a word and ci is a "borrow in". The borrow out is returned. 
*/
word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a - b * c where a and b are m words in length, and c 
   is a word. The borrow out is returned. 
*/
#define nn_submul1(a, b, m, c) \
   nn_submul1_c(a, b, m, c, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must be
   reduced mod d. The quotient q requires m words of space.  An 
   exception will result if d is 0.
*/
word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci);

/*
   Set q = a / d and return the remainder, where a is m words in 
   length and d is a word. The quotient q requires m words of space. 
   An exception will result if d is 0.
*/
#define nn_divrem1_simple(q, a, m, d) \
   nn_divrem1_simple_c(q, a, m, d, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must 
   be reduced mod d. The quotient q requires m words of space.  
   Requires that inv is a precomputed inverse of d computed by the 
   function precompute_inverse1. Also requires that d is normalised, 
   i.e with most significant bit set.
*/
word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t inv, word_t ci);

/* 
   As per _nn_divrem1_preinv_c but with no carry-in.
*/
#define nn_divrem1_preinv(q, a, m, d, inv) \
   nn_divrem1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Computes r and q such that r * B^m + a = q * d + ci, where
   q and a are m words in length, d is an odd word_t, inv is a
   precomputed Hensel inverse of d computed by the function
   precompute_hensel_inverse1 and ci is a "carry-in" word.
*/
word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci);

/*
   As per _nn_divrem_hensel1_preinv_c but with no "carry-in".
*/
#define nn_divrem_hensel1_preinv(q, a, m, d, inv) \
   nn_divrem_hensel1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Return a + ci * B^m mod d given a precomputed mod_preinv1_t,
   where ci is a "carry-in" word and a is m words in length, 
   that ci is reduced mod d and that d is not zero.
*/
word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci);

/* 
   Return a mod d given a precomputed mod_preinv1_t, where a is m 
   words in length and that d is not zero.
*/
#define nn_mod1_preinv(a, m, d, inv) \
   nn_mod1_preinv_c(a, m, d, inv, (word_t) 0)

/**********************************************************************
 
    Comparison

**********************************************************************/

/* 
   Return 1 if the m words at a match the n words at b,
   otherwise return 0.
*/
static inline
int nn_equal_m(nn_src_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      if (a[i] != b[i]) return 0;

   return 1;
}

/* 
   Return 1 if am == bm and the am words at a match the am 
   words at b, otherwise return 0.
*/
static inline
int nn_equal(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
      return 0;
   else 
      return nn_equal_m(a, b, am);
}

/* 
   Return a positive value if {a, m} > {b, m}, a negative
   value if {a, m} < {b, m}, otherwise returns 0.
*/
int nn_cmp_m(nn_src_t a, nn_src_t b, len_t m);

/* 
   Return a positive value if {a, am} > {b, bm}, a negative
   value if {a, am} < {b, bm}, otherwise returns 0.
*/
static inline
int nn_cmp(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
   {
      if (am > bm)
         return 1;
      else
         return -1;
   } else
      return nn_cmp_m(a, b, am);
}

/**********************************************************************

    Bit operations

**********************************************************************/

/* 
   Set b to the bitwise logical not of a.
*/
static inline
void nn_not(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = ~b[i];
}

/*
   Return the count of bits in a that are set to 1.
*/
static inline
bits_t nn_popcount(nn_src_t a, len_t m)
{
   len_t i;
   bits_t count = 0;

   for (i = 0; i < m; i++)
      count += popcount_bits(a[i]);

  return count;
}

/*
   After the low 'skip' bits, find the next bit position that is set to 1,
   or return -1 if there is no bit set to 1 after b.
   It must hold that skip < m.
*/
bits_t nn_scan1(bits_t skip, nn_src_t a, len_t m);

/*
   Return the Hamming distance of {a, m} and {b, m}.
   Equivalent to popcount(a ^ b).
*/
static inline
bits_t nn_hamdist_m(nn_src_t a, nn_src_t b, len_t m)
{
   len_t i;
   bits_t count = 0;

   for (i = 0; i < m; i++)
     count += popcount_bits(a[i] ^ b[i]);

  return count;
}

/* Bitwise xor. */
static inline
void nn_xor_m(nn_t a, nn_src_t b, nn_src_t c, len_t m)
{
   len_t i;
   for (i = 0; i < m; i++)
     a[i] = b[i] ^ c[i];
}

/**********************************************************************
 
    Internal helper functions

**********************************************************************/

/*
   Let S = mulmid(a, b1 + b2) and T = mulmid(a, b1) + mulmid(a, b2), 
   where a is of length 2*n - 1 and b1 and b2 are of length n. This 
   function returns T - S. The sum of b1 and b2 is returned in 
   {r, n}. Any carry out is returned by the function. We require 
   n >= 2.

*/
word_t _nn_mulmid_add_rfix_m(nn_t r, nn_t ov, nn_t p,
                      nn_src_t a, nn_src_t b1, nn_src_t b2, len_t n);

/*
   Let S = mulmid(a, b1 - b2) and T = mulmid(a, b1) - mulmid(a, b2), 
   where a is of length 2*n - 1 and b1 and b2 are of length n. This 
   function returns T - S. The sum of b1 and b2 is returned in 
   {r, n}. Any borrow out is returned by the function. We require 
   n >= 2.

*/
word_t _nn_mulmid_sub_rfix_m(nn_t r, nn_t ov, nn_t p,
                      nn_src_t a, nn_src_t b1, nn_src_t b2, len_t n);

/*
   Let S = mulmid(a1 + a2, b) and T = mulmid(a1, b) + mulmid(a2, b), 
   where a1, a2 are of length 2*n - 1 and b is of length n. This 
   function returns T - S. The sum of a1 and a2 is returned in 
   {r, 2*n - 1}. Any carry out is returned by the function. We 
   require n >= 2.
*/
word_t _nn_mulmid_add_lfix_m(nn_t r, nn_t ov, nn_t p,
             nn_src_t a1, nn_src_t a2, nn_src_t b, len_t n);

/*
   Set sl words of q to ~WORD(0) and subtract 
   sum_{i = 0)^{s - 1} q[i]*{d + s - i - 1, i + 2} from a.
   Return a[2].
*/
word_t _nn_divapprox_helper(nn_t q, nn_t a, nn_src_t d, len_t s);

/**********************************************************************
 
    Quadratic arithmetic functions

**********************************************************************/

/*
   Set {r, m1 + m2} = {a, m1} * {b, m2}. 
   The output r may not alias either of the inputs a or b. We require 
   m1, m2 > 0.
*/
void nn_mul_classical(nn_t r, nn_src_t a, len_t m1, nn_src_t b, len_t m2);

/*
   Set ov*B^m1 + {r, m1} to sum_{i + j < m1} a[i]*b[j]*B^{i + j}. In 
   other words, {r, m1} will be the low m1 words of the product 
   {a, m1}*{b, m2} and ov will contain any overflow from word by word 
   multiplications that don't fit in the m1 words of r and is 2 words
   in length. The output r may not alias either of the inputs a or b. 
   We require m1 >= m2 > 0.
*/
void nn_mullow_classical(nn_t ov, nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2);

/*
   Set r to ov + sum_{i + j > m1} a[i]*b[j]*B^{i + j - m1}. In other
   words, this returns the high part of the multiplication {a, m1}*
   {b, m2}. The output requires m2 words. We require m1 >= m2 > 0. 
   If ov from mullow is passed to mulhigh then a mullow followed by a 
   mulhigh is the same as a full mul. The output r may not alias 
   either a or b.
*/
void nn_mulhigh_classical(nn_t r, nn_src_t a, len_t m1, 
                                       nn_src_t b, len_t m2, nn_t ov);

/*
   Set ov, {p, m - n + 1} to the middle product of {a, m} and {b, n}, i.e.
   set p to sum_{m - 1 >= i + j >= n - 1} a[i]*b[j]*B^{i + j - n + 1}, with 
   overflows accumulating in the two word ov. We require m + 1 >= n >= 2.
   The output p may not alias either a or b.
*/
void nn_mulmid_classical(nn_t ov, nn_t p,
                            nn_src_t a, len_t m, nn_src_t b, len_t n);


/*
   Given a of length m and d of length n with a carry-in ci, compute
   the quotient of ci*B^m + a by d, and leave the remainder in the bottom 
   n words of a. Requires m >= n > 0. The precomputed inverse inv should 
   be computed from the leading two words of d (or the leading word and 0 
   if n is 1) using precompute_inverse_lead. If a_n is the leading n words 
   of a, then ci*B^n + a_n must be less than B * d. The quotient
   must have space for m - n + 1 words. The quotient may not alias a or d
   and a must not alias d. The leading word of d must be normalised, i.e. 
   d[n - 1] >= B/2.
*/
void nn_divrem_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                     len_t n, preinv2_t inv, word_t ci);

/*
   As per nn_divrem_classical_preinv_c however only a partial remainder is
   computed. It is equal to a - sum_{i + j >= m - s - 1} q_i*d_j*B^{i+j} 
   where q_i are the s = m - n + 1 words of the quotient and d_j are the 
   words of d. If q1 is the actual quotient and q2 the computed quotient, 
   then we have q1 + 1 >= q2 >= q1. The partial remainder is stored in 
   place in a, the high words of a being destroyed. 
   We require that q may not alias a or d and a must not alias d.
   Due to successive truncation, it is possible that the partial remainder 
   may exceed d, in which case the extra word is returned by this function.
   The leading word of d must be normalised, i.e. d[n - 1] >= B/2.
*/
word_t nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, 
                         nn_src_t d,len_t n, preinv2_t inv, word_t ci);

/*
   Perform Hensel division of {a, m} by {d, n} with the quotient mod B^m
   being returned in q and any overflow from mullow(q, d) being returned
   in ov. We require m >= n > 0. The quotient q requires m words of space
   and ov requires 2 words. The dividend, a, is destroyed. We also require 
   that q does not alias a or d and a must not alias d. The divisor d must 
   be odd and inv must be a precomputed inverse of d[0] computed with 
   precompute_hensel_inverse1.
*/
void nn_div_hensel_preinv(nn_t ov, nn_t q, nn_t a, len_t m, 
                            nn_src_t d, len_t n, hensel_preinv1_t inv);

/*
   Compute the gcd g of {a, m} and {b, n}. We assume m >= n > 0. The value
   g must have enough space for n words, but the actual number used is
   returned by the function. Both a and b must have space for m limbs.
   The inputs a and b are destroyed. The inputs a and b may not be aliased
   and must not have leading words that are zero.
*/
len_t nn_gcd_lehmer(nn_t g, nn_t a, len_t m, nn_t b, len_t n);

/*
   As per nn_gcd_lehmer except that an additonal value v is computed
   such that u' = g + bv is divisible by a. If u = u'/a then the identity
   ua - bv = g is satisfied. The value v computed will be reduced modulo
   a. We require v to have enough space for m words. Both a and b must have
   space for m limbs. The inputs a and b are destroyed. The inputs a and
   b may not be aliased and must not have leading words that are zero.
*/
len_t nn_xgcd_lehmer(nn_t g, nn_t v, nn_t a, len_t m, nn_t b, len_t n);

/**********************************************************************
 
    Subquadratic arithmetic functions

**********************************************************************/

/*
   Perform Karatsuba multiplication of {a, m}, {b, n}.
   
   Assumes n > 1 and m >= n >= (m + 1)/2
   No overlap between src and dst
 
   Algorithm: 
      z = ah*bh*B^2l + ((ah + al)*(bh + bl) - ah*bh - al*bl)*B^l + al*bl
*/
void nn_mul_kara(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   Perform Toom-3 multiplication of {a, m}, {b, n}.
   
   Assumes n > 2 and m >= n > 2*((m + 2)/3)
   No overlap between src and dst
 
   Algorithm: 
      + Split a and b into 3 parts of lengths m3, m3, h1 and m3, m3, h2
        respectively, where m3 = (m + 2)/3
      + Evaluate split a and b at 1, 2, 4, 0, oo
      + Pointwise multiplication
      + Interpolate 5 part result
      + Recombine 5 parts into one result by evaluation at B^m3
*/
void nn_mul_toom33(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   Perform Toom-3,2 multiplication of {a, m}, {b, n}.
   
   Assumes n > 4 and 2*((m + 2)/3) >= n > (m + 2)/3
   No overlap between src and dst
 
   Algorithm: 
      + Split a into 3 parts and b into two parts of lengths m3, m3, h1 and 
        m3, h2 respectively, where m3 = (m + 2)/3
      + Evaluate split a and b at 1, 2, 0, oo
      + Pointwise multiplication
      + Interpolate 4 part result
      + Recombine 4 parts into one result by evaluation at B^m3
*/
void nn_mul_toom32(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   As per nn_mulmid_classical, except that we require m >= 2*n - 1.
*/
void nn_mulmid_kara(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   As per nn_mullow_classical, except that m = n and n >= 2.
*/
void nn_mullow_kara_m(nn_t ov, nn_t p, nn_src_t a, nn_src_t b, len_t n);

/*
   As per nn_mullow_classical.
*/
void nn_mullow_kara(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   As per nn_divrem_classical_preinv_c however only a partial remainder is
   computed and we require n >= 2. The partial remainder is equal to 
   a - sum_{i + j >= m - s - 1} q_i*d_j*B^{i+j} where the q_i are the 
   s = m - n + 1 words of the quotient and d_j are the words of d. 
   If q1 is the actual quotient and q2 the computed quotient, then we have 
   q1 + 1 >= q2 >= q1 - 1. The partial remainder is stored in the low 
   m - s + 1 words of a, the other words being destroyed. Due to successive 
   truncation, it is possible that the partial remainder may exceed d, in 
   which case the extra word is returned by this function.
*/
word_t nn_divapprox_divconquer_preinv_c(nn_t q, nn_t a, len_t m, 
                     nn_src_t d, len_t n, preinv2_t dinv, word_t ci);

/*
   As per nn_divrem_classical_preinv_c except that n >= 2.
*/
void nn_divrem_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv2_t dinv, word_t ci);

/*
   As per nn_divrem_classical_preinv_c but returning quotient only and we
   require n >= 2. The value of a is destroyed.
*/
void nn_div_divconquer_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                        len_t n, preinv2_t dinv, word_t ci);

/**********************************************************************
 
    Tuned (best-of-breed) arithmetic functions

**********************************************************************/

/*
   Set {p, 2*m} = {a, m} * {b, m}. 
   The output p may not alias either of the inputs a or b. We require 
   m > 0.
*/
void nn_mul_m(nn_t p, nn_src_t a, nn_src_t b, len_t m);

/*
   Set {p, m + n} = {a, m} * {b, n}. 
   The output p may not alias either of the inputs a or b. We require 
   m, n > 0.
*/
void nn_mul(nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   As per nn_mullow_classical, but with m = n.
*/
static __inline__
void nn_mullow_m(nn_t ov, nn_t p, nn_src_t a, nn_src_t b, len_t n)
{   
   if (n <= MULLOW_CLASSICAL_CUTOFF)
      nn_mullow_classical(ov, p, a, n, b, n);
   else
      nn_mullow_kara_m(ov, p, a, b, n);
}

/*
   As per nn_mullow_classical.
*/
void nn_mullow(nn_t ov, nn_t p, nn_src_t a, len_t m, nn_src_t b, len_t n);

/*
   As per nn_divapprox_classical_preinv_c.
*/
static __inline__
word_t nn_divapprox_preinv_c(nn_t q, nn_t a, len_t m, 
                          nn_src_t d, len_t n, preinv2_t dinv, word_t ci)
{
   ASSERT(DIVAPPROX_CLASSICAL_CUTOFF >= 3);
   
   if (m - n + 1 <= DIVAPPROX_CLASSICAL_CUTOFF || n == 1)
      return nn_divapprox_classical_preinv_c(q, a, m, d, n, dinv, ci);
   else
      return nn_divapprox_divconquer_preinv_c(q, a, m, d, n, dinv, ci);
}

/*
   As per nn_divrem_classical_preinv_c.
*/
static __inline__
void nn_divrem_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                    len_t n, preinv2_t dinv, word_t ci)
{
   if (n <= DIVREM_CLASSICAL_CUTOFF || m == n)
      nn_divrem_classical_preinv_c(q, a, m, d, n, dinv, ci);
   else
      nn_divrem_divconquer_preinv_c(q, a, m, d, n, dinv, ci);
}

/*
   Set {q, m - n + 1} to the quotient of {a, m} by {d, n} and set 
   {a, n} to the remainder. Requires m >= n > 0.
   Aliasing of q with a or d or aliasing of a and d is not 
   permitted.
*/
void nn_divrem(nn_t q, nn_t a, len_t m, nn_src_t d, len_t n);

/*
   Set {q, m - n + 1} to the quotient of {a, m} by {d, n}. 
   The value of a is destroyed. Requires m >= n > 0.
   Aliasing of q with a or d or aliasing of a and d is not 
   permitted.
*/
void nn_div(nn_t q, nn_t a, len_t m, nn_src_t d, len_t n);

#define nn_gcd(g, a, m, b, n) \
   nn_gcd_lehmer(g, a, m, b, n)

#define nn_xgcd(g, v, a, m, b, n) \
   nn_xgcd_lehmer(g, v, a, m, b, n)

#ifdef __cplusplus
 }
#endif

#endif

