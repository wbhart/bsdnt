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

word_t _nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
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


