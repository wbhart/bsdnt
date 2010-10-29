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

   TEST_START(1, ITER) /* test a * (b + c) = a * b + a * c */
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

      if (!result) 
      {
         print_debug(a, m); print_debug(b, n); print_debug(c, n);  print_debug(s, n + 1);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_muladd_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, c, r1, r2;

   printf("muladd_classical...");

   TEST_START(1, ITER) /* test a + b * c = muladd(a, b, c) */
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

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); print_debug(c, n);
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   TEST_START(aliasing, ITER) /* test aliasing of r and a in muladd */
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

      if (!result) 
      {
         print_debug(a, m); print_debug(b, m); print_debug(c, n);
         print_debug_diff(r1, r2, m + n);
      }
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

   TEST_START(1, ITER) /* test (a * d) / d = a remainder 0 */
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

      if (!result) 
      {
         print_debug(a, m); print_debug(q, m); print_debug(d, n);  print_debug(r1, n);
         print_debug_diff(q, a, m);
      }
   } TEST_END;

   TEST_START(2, ITER) /* test (a * d + s) / d = a remainder s */
   {
      randoms_upto(30, NONZERO, state, &n, NULL);
      randoms_upto(n + 1, NONZERO, state, &m, NULL);
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

      if (!result) 
      {
         printf("inv.norm = %ld, inv.dinv = %lx, inv.d1 = %lx\n", 
            inv.norm, inv.dinv, inv.d1);
         print_debug(a, m); print_debug(q, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q, a, m);
         print_debug_diff(s, r1, m);
      }
   } TEST_END;

   return result;
}

int test_divapprox_classical_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, r1, r2, s, q1, q2, d;
   preinv1_2_t inv;
   
   printf("divapprox_classical_preinv...");

   TEST_START(1, ITER) /* test divapprox is at most one more than divrem */
   {
      randoms_upto(30, NONZERO, state, &n, NULL);
      n++; /* require n >= 2 */
      randoms_upto(n + 1, NONZERO, state, &m, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q1, &q2, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      /* ensure s is reduced mod d */
      do {
         randoms_of_len(n, ANY, state, &d, &s, NULL);
      } while (nn_cmp_m(d, s, n) <= 0);
      
      nn_s_muladd_classical(r1, s, d, n, a, m);
      nn_copy(r2, r1, m + n);

      precompute_inverse1_2(&inv, d[n - 1], (n == 1) ? (word_t) 0 : d[n - 2]);
      nn_r_divrem_classical_preinv(q1, r1, m + n - 1, d, n, inv);

      nn_r_divapprox_classical_preinv(q2, r2, m + n - 1, d, n, inv);

      result = nn_equal_m(q1, q2, m);
      nn_add1(q1, q1, m, 1);
      result |= nn_equal_m(q1, q2, m);

      if (!result) 
      {
         printf("inv.norm = %ld, inv.dinv = %lx, inv.d1 = %lx\n", 
            inv.norm, inv.dinv, inv.d1);
         print_debug(a, m); print_debug(q1, m); print_debug(d, n);  print_debug(s, n);
         print_debug_diff(q1, q2, m);
      }
   } TEST_END;

   return result;
}

int test_mullow_classical(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, b, r1, r2, ov;
   
   printf("mullow_classical...");

   TEST_START(1, ITER) /* test a * (b + c) = a * b + a * c */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ANY, state, &b, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &r2, NULL);
      
      nn_s_mul_classical(r1, a, m, b, n);
      
      nn_mullow_classical(ov, r2, a, m, b, n);
      nn_s_mulhigh_classical(r2 + m, a, m, b, n, ov);
      
      result = nn_equal_m(r1, r2, m + n);

      if (!result) 
      {
         printf("ov[0] = %lx, ov[1] = %lx\n", ov[0], ov[1]);
         print_debug(a, m); print_debug(b, n); 
         print_debug_diff(r1, r2, m + n);
      }
   } TEST_END;

   return result;
}

int test_div_hensel_preinv(void)
{
   int result = 1;
   len_t m, n;
   nn_t a, a1, a2, r1, q, q1, q2, d, ov, ov2, t;
   hensel_preinv1_t inv;
   
   printf("div_hensel_preinv...");

   TEST_START(1, ITER) /* test (a * d) / d = a */
   {
      randoms_upto(30, NONZERO, state, &m, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, NULL);
      randoms_of_len(n, ODD, state, &d, NULL);
      randoms_of_len(m + n, ANY, state, &r1, &q, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      
      if (m >= n) nn_s_mul_classical(r1, a, m, d, n);
      else nn_s_mul_classical(r1, d, n, a, m);

      precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q, r1, m + n, d, n, inv);

      result = (nn_equal_m(q, a, m) && nn_normalise(q, m + n) == m);

      if (!result) printf("m = %ld, n = %ld\n", m, n);
   } TEST_END;

   TEST_START(1, ITER) /* test inexact division yields correct overflow */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(m, ANY, state, &a, &q, NULL);
      randoms_of_len(n, ODD, state, &d, NULL);
      randoms_of_len(2, ANY, state, &ov, &ov2, NULL);
      
      precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q, a, m, d, n, inv);

      nn_mullow_classical(ov2, a, q, m, d, n);

      result = (nn_equal_m(ov, ov2, 2));

      if (!result) 
      {
         printf("inv = %lx\n", inv);
         print_debug(a, m); print_debug(q, m); print_debug(d, n); 
         print_debug_diff(ov, ov2, 2L);
      }
   } TEST_END;

   TEST_START(chaining, ITER) /* test chaining of div_hensel */
   {
      randoms_upto(30, NONZERO, state, &m, NULL);
      randoms_upto(m + 1, NONZERO, state, &n, NULL);
      
      randoms_of_len(2*m, ANY, state, &a1, &a2, &q, NULL);
      randoms_of_len(2*m, ANY, state, &q1, &q2, NULL);
      randoms_of_len(n, ODD, state, &d, &t, NULL);
      randoms_of_len(2, ANY, state, &ov, NULL);
      
      nn_copy(a2, a1, 2*m);

      precompute_hensel_inverse1(&inv, d[0]);
      nn_div_hensel_preinv(ov, q1, a1, m, d, n, inv);
      nn_s_mulhigh_classical(t, q1, m, d, n, ov);
      nn_sub(a1 + m, a1 + m, m, t, n);
      nn_div_hensel_preinv(ov, q1 + m, a1 + m, m, d, n, inv);

      nn_div_hensel_preinv(ov, q2, a2, 2*m, d, n, inv);

      result = (nn_equal_m(q1, q2, 2*m));

      if (!result) 
      {
         printf("inv = %lx, ov[0] = %lx, ov[1] = %lx\n", inv, ov[0], ov[1]);
         print_debug(a1, 2*m); print_debug(a2, 2*m); print_debug(q, 2*m); 
         print_debug(q1, 2*m); print_debug(q2, 2*m); print_debug(d, n); print_debug(t, n); 
         print_debug_diff(q1, q2, 2*m);
      }
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
   RUN(test_mullow_classical);
   RUN(test_divrem_classical_preinv);
   RUN(test_divapprox_classical_preinv);
   RUN(test_div_hensel_preinv);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   randclear(state);

   return 0;
}


