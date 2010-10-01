#include "nn.h"
#include "nn_linear_arch.h"

/**********************************************************************
 
    Random generation

**********************************************************************/

#if WORD_BITS == 64

word_t __randval = 4035456057UL;
word_t __randval2 = 6748392731UL;

void randinit(rand_t state)
{
}

void randclear(rand_t state)
{
}

word_t randword(rand_t state) 
{   
    __randval = (__randval*1025416097UL + 286824428UL)%4294967311UL;
    __randval2 = (__randval2*1647637699UL + 286824428UL)%4294967357UL;

    return __randval + (__randval2<<32);
}

#else

word_t __randval = 3119766748UL;
word_t __randval2 = 4225528843UL;

word_t randword(rand_t state) 
{   
    __randval = (__randval*1573677563UL +  1626832774UL)%65537UL;
    __randval2 = (__randval2*897228705UL +  1626832774UL)% 65539UL;

    return __randval + (__randval2<<16);
}

#endif

word_t randint(word_t m, rand_t state)
{
   return (randword(state) % m);
}

void nn_random(nn_t a, rand_t state, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = randword(state);
}

/**********************************************************************
 
    Linear arithmetic functions

**********************************************************************/

#ifndef HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] + (dword_t) c[i] + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] - (dword_t) c[i] - (dword_t) bi;
      a[i] = (word_t) t;
      bi = -(t >> WORD_BITS);
   }

   return bi;
}

#endif

#ifndef HAVE_ARCH_nn_shl_c

word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (((dword_t) b[i]) << bits);
      a[i] = (word_t) t + ci;
      ci = (t >> WORD_BITS); 
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_shr_c

word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   dword_t t;
   long i;
   bits = WORD_BITS - bits;

   for (i = m - 1; i >= 0L; i--)
   {
      t = (((dword_t) b[i]) << bits);
      a[i] = (t >> WORD_BITS) + ci;
      ci = (word_t) t; 
   }

   return ci;
}

#endif

word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t c)
{
   dword_t t;
   long i;

   for (i = 0; i < m && c != 0; i++)
   {
      t = (dword_t) b[i] + (dword_t) c;
      a[i] = (word_t) t;
      c = (t >> WORD_BITS);
   }

   if (a != b)
      for ( ; i < m; i++)
         a[i] = b[i];

   return c;
}

word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c)
{
   dword_t t;
   long i;

   for (i = 0; i < m && c != 0; i++)
   {
      t = (dword_t) b[i] - (dword_t) c;
      a[i] = (word_t) t;
      c = -(t >> WORD_BITS);
   }

   if (a != b)
      for ( ; i < m; i++)
         a[i] = b[i];

   return c;
}

#ifndef HAVE_ARCH_nn_neg_c

word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
{
   dword_t t;
   long i;
   
   ci = 1 - ci;

   for (i = 0; i < m && ci != 0; i++)
   {
      t = (dword_t) ~b[i] + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   for ( ; i < m; i++)
      a[i] = ~b[i];

   return (word_t) 1 - ci;
}

#endif

#ifndef HAVE_ARCH_nn_mul1_c

word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) b[i] * (dword_t) c + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_addmul1_c

word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] + (dword_t) b[i] * (dword_t) c + (dword_t) ci;
      a[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_muladd1_c

word_t nn_muladd1_c(nn_t r, nn_src_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] + (dword_t) b[i] * (dword_t) c + (dword_t) ci;
      r[i] = (word_t) t;
      ci = (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_submul1_c

word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   dword_t t;
   long i;

   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] - (dword_t) b[i] * (dword_t) c - (dword_t) ci;
      a[i] = (word_t) t;
      ci = -(t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_simple_c

word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci)
{
   dword_t t;
   long i;

   for (i = m - 1; i >= 0; i--)
   {
      t = (((dword_t) ci) << WORD_BITS) + (dword_t) a[i];
      q[i] = t / (dword_t) d;
      ci = (word_t) (t % (dword_t) d);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_preinv_c

word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t inv, word_t ci)
{
   dword_t t;
   long i;
   word_t norm = inv.norm;
   word_t dinv = inv.dinv;
   
   d <<= norm;
   ci <<= norm;

   for (i = m - 1; i >= 0; i--)
   {
      t = (((dword_t) ci) << WORD_BITS) + (((dword_t) a[i]) << norm);
      divrem21_preinv1(q[i], ci, t, d, dinv);
   }

   return (ci >> norm);
}

#endif

#ifndef HAVE_ARCH_nn_divrem_hensel1_preinv_c

word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci)
{
   long i;
   dword_t t, r;
   
   for (i = 0; i < m; i++)
   {
      t = (dword_t) a[i] - (dword_t) ci;
      q[i] = (word_t) t * inv;
      r = (dword_t) q[i] * (dword_t) d;
      ci = (word_t) (r >> WORD_BITS) - (word_t) (t >> WORD_BITS);
   }

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mod1_preinv_c

word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci)
{
   dword_t t, u;
   word_t a1, a0;
      
   if (m & 1) /* odd number of words plus carry word */
   {
      a1 = ci;
      a0 = a[--m];
   } else /* even number of words plus carry word */
   {
      if (m == 0) return ci;

      m -= 2;
      t = (dword_t) a[m] + (((dword_t) a[m + 1]) << WORD_BITS);
      u = t + (dword_t) ci * (dword_t) inv.b2;
      if (u < t) u += (dword_t) inv.b2;
      a1 = (word_t) (u >> WORD_BITS);
      a0 = (word_t) u;
   }

   /* reduce to two words */
   while (m >= 2)
   {
      m -= 2;
      u = (dword_t) a[m] + (((dword_t) a[m + 1]) << WORD_BITS);
      t = u + (dword_t) a0 * (dword_t) inv.b2;
      if (t < u) t += (dword_t) inv.b2;
      u = t + (dword_t) a1 * (dword_t) inv.b3;
      if (u < t) u += (dword_t) inv.b2;
      a1 = (word_t) (u >> WORD_BITS);
      a0 = (word_t) u;
   }
   
   /* reduce top word mod d */
   u = (dword_t) a0 + (dword_t) a1 * (dword_t) inv.b1;

   return u % (dword_t) d;
}

#endif

/**********************************************************************
 
    Comparison

**********************************************************************/

#ifndef HAVE_ARCH_nn_cmp_c

int nn_cmp_m(nn_src_t a, nn_src_t b, len_t m)
{
   while (m--)
   {
      if (a[m] != b[m])
      {
         if (a[m] > b[m])
            return 1;
         else
            return -1;
      }
   }

   return 0;
}

#endif