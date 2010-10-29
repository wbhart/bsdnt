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

#ifndef HAVE_ARCH_nn_divrem_classical_preinv_c

void nn_divrem_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_2_t inv, word_t ci)
{
   dword_t t;
   long i, j = m - n;
   word_t q1, rem;
   word_t norm = inv.norm;
   word_t dinv = inv.dinv;
   word_t d1 = inv.d1;

   for (i = m - 1; i >= n - 1; i--, j--)
   {
      /* top "two words" of remaining dividend, shifted */
      if (norm)
		  t = (((((dword_t) ci) << WORD_BITS) + (dword_t) a[i]) << norm) 
		        + (dword_t) (a[i-1] >> (WORD_BITS - norm));
	  else
	      t = (((dword_t) ci) << WORD_BITS) + (dword_t) a[i];
      
      /* check for special case, a1 == d1 which would cause overflow */
      if ((t >> WORD_BITS) == d1) 
		  q1 = ~(word_t) 0;
      else 
		  divrem21_preinv1(q1, rem, t, d1, dinv);

	  /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q1);
      
      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a + j, a + j, d, n);
      }

      q[j] = q1;
      ci = a[i];
   }
}


#endif

#ifndef HAVE_ARCH_nn_divapprox_classical_preinv_c

void nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_2_t inv, word_t ci)
{
   dword_t t;
   long i = m - 1, j = m - n;
   word_t q1, rem;
   word_t norm = inv.norm;
   word_t dinv = inv.dinv;
   word_t d1 = inv.d1;
   len_t s = m - n; /* this many iterations to get to last quotient */
   s = 2 + s + (d[n-1] <= 2*s); /* need two normalised words at that point */
   if (s > i + 1) s = i + 1; /* ensure we don't do too many iterations */
   
   for ( ; s >= n; i--, j--, s--)
   {
      /* top "two words" of remaining dividend, shifted */
      if (norm)
		  t = (((((dword_t) ci) << WORD_BITS) + (dword_t) a[i]) << norm) 
		        + (dword_t) (a[i - 1] >> (WORD_BITS - norm));
	  else
		  t = (((dword_t) ci) << WORD_BITS) + (dword_t) a[i];
      
      /* check for special case, a1 == d1 which would cause overflow */
      if ((t >> WORD_BITS) == d1) q1 = ~(word_t) 0;
      else divrem21_preinv1(q1, rem, t, d1, dinv);

      /* a -= d*q1 */
      ci -= nn_submul1(a + j, d, n, q1);
      
      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a + j, a + j, d, n);
      }
	  
      q[j] = q1;
      ci = a[i];
   }
   
   d = d + n - s; /* make d length s */
   a = a + i + 1 - s; /* make a length s + carry */
   
   for ( ; i >= n - 1; i--, j--, s--)
   {
      /* top "two words" of remaining dividend, shifted */
      if (norm)
		  t = (((((dword_t) ci) << WORD_BITS) + (dword_t) a[s - 1]) << norm) 
		        + (dword_t) (a[s - 2] >> (WORD_BITS - norm));
	  else
		  t = (((dword_t) ci) << WORD_BITS) + (dword_t) a[s - 1];
      
      /* check for special case, a1 == d1 which would cause overflow */
      if ((t >> WORD_BITS) == d1) 
          q1 = ~(word_t) 0;
      else 
          divrem21_preinv1(q1, rem, t, d1, dinv);
	  
      /* a -= d*q1 */
      ci -= nn_submul1(a, d, s, q1);
      
	  if (ci == 1)
		  ci -= nn_sub1(a + 1, a + 1, s - 1, ~0UL);

      /* correct if remainder has become negative */
      while (ci)
      {
         q1--;
         ci += nn_add_m(a, a, d, s);
      }
	  
      q[j] = q1;
      ci = a[s - 1];
      d++;
   }   
}

#endif
