#include "nn.h"

void nn_mul_classical(nn_t r, nn_src_t a, len_t m1, nn_src_t b, len_t m2)
{
   len_t i;
  
   if (m1) nn_s_mul1(r, b, m2, a[0]); 
   
   for (i = 1; i < m1; i++)
      r[m2 + i] = nn_addmul1(r + i, b, m2, a[i]);
}
