#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "test.h"

#define ITER 30000

rand_t state;

int test_mul_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, c, r1, r2, s;

   printf("mul_classical...");

   TEST_START(ITER) /* test a * (b + c) = a * b + a * c */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n + 1, ANY, state, &s, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      do {
         randoms_of_len(n, ANY, state, &b, &c, NULL);
         nn_s_add_m(s, b, c, n);
      } while (s[n]);
      
      nn_s_mul_classical(r1, a, m, b, n);
      nn_s_mul_classical(r2, a, m, c, n);
      nn_add_m(r2, r2, r1, m + n);

      nn_s_mul_classical(r1, a, m, s, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   return result;
}

int test_muladd_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, c, r1, r2;

   printf("muladd_classical...");

   TEST_START(ITER) /* test a + b * c = muladd(a, b, c) */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(n, ANY, state, &c, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_s_mul_classical(r1, b, m, c, n);
      nn_add(r1, r1, m + n, a, m);
      
      nn_s_muladd_classical(r2, a, b, m, c, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   TEST_START(ITER) /* test aliasing of r and a in muladd */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, &b, NULL);
      randoms_of_len(n, ANY, state, &c, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_copy(r1, a, m);
      nn_s_muladd_classical(r1, r1, b, m, c, n);
      
      nn_s_muladd_classical(r2, a, b, m, c, n);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   return result;
}

int test_divrem_classical_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, s, q, d;
   preinv1_2_t inv;
   
   printf("divrem_classical_preinv...");

   TEST_START(ITER) /* test (a * d) / d = a remainder 0 */
   {
      randoms_upto(30, NONZERO, state, &m, &n, NULL);
      n++; /* require n >= 2 */

      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(n, ANY, state, &d, NULL);
      randoms_of_len(m + n, ANY, state, &r1, NULL);
      
      if (m >= n) nn_s_mul_classical(r1, a, m, d, n);
      else nn_s_mul_classical(r1, d, n, a, m);

      precompute_inverse1_2(&inv, d[n - 1], d[n - 2]);
      nn_r_divrem_classical_preinv(q, r1, m + n - 1, d, n, inv);

      result = (nn_equal_m(q, a, m) && nn_normalise(r1, n) == 0);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   TEST_START(ITER) /* test (a * d + s) / d = a remainder s */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      n++; /* require n >= 2 */

      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(m + n, ANY, state, &r1, NULL);
      
      /* ensure s is reduced mod d */
      do {
         randoms_of_len(n, ANY, state, &d, &s, NULL);
      } while (nn_cmp_m(d, s, n) <= 0);
      
      nn_s_muladd_classical(r1, s, d, n, a, m);
      
      precompute_inverse1_2(&inv, d[n - 1], d[n - 2]);
      nn_r_divrem_classical_preinv(q, r1, m + n - 1, d, n, inv);

      result = (nn_equal_m(q, a, m) && nn_equal_m(s, r1, n));

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   return result;
}

int main(void)
{
   long pass = 0;
   long fail = 0;
   
   randinit(state);

   RUN(test_mul_classical);
   RUN(test_muladd_classical);
   RUN(test_divrem_classical_preinv);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   randclear(state);

   return 0;
}


