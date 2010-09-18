#include "nn.h"

word_t nn_mul_classical(nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2)
{
   len_t i;
   word_t ci = 0;
  
   if (m1) ci = nn_mul1(r, b, m2, a[0]); 
   
   for (i = 1; i < m1; i++)
   {
      r[m2 + i - 1] = ci;
      ci = nn_addmul1(r + i, b, m2, a[i]);
   }

   return ci;
}

word_t nn_muladd_classical(nn_t r, nn_src_t a, nn_src_t b, 
                                     len_t m1, nn_src_t c, len_t m2)
{
   len_t i;
   word_t ci = 0;
  
   if (m2) ci = nn_muladd1(r, a, b, m1, c[0]); 
   else if (m1)
   {
      if (r != a) nn_copy(r, a, m1 - 1);
      return a[m1 - 1];
   }

   for (i = 1; i < m2; i++)
   {
      r[m1 + i - 1] = ci;
      ci = nn_addmul1(r + i, b, m1, c[i]);
   }

   return ci;
}
