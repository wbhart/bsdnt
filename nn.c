#include "nn.h"

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

word_t _nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi)
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

word_t _nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
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

word_t _nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
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

word_t _nn_add1(nn_t a, nn_src_t b, len_t m, word_t c)
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

word_t _nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c)
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

word_t _nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
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

word_t _nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
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

word_t _nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
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

word_t _nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
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

word_t _nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci)
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

/**********************************************************************
 
    Comparison

**********************************************************************/

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
