/* 
  Copyright (C) 2010, 2013 William Hart
  Copyright (C) 2010, Brian Gladman
  Copyright (C) 2010, Antony Vennard
  Copyright (C) 2010, Gonzalo Tornaria

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



#ifdef __cplusplus
 extern "C" {
#endif

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "jrcxz 3f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    bt  $0, %%rax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    adc (%%rdx,%%rcx,8), %%r10; \
    mov %%r10, (%%rdi,%%rcx,8); \
    leaq 1(%%rcx), %%rcx; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc %%al; \
3:;"

   : "=a" (ci)
   : "a" (ci), "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc
#define HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "jrcxz 3f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    bt  $0, %%rax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    sbb (%%rdx,%%rcx,8), %%r10; \
    movq %%r10, (%%rdi,%%rcx,8); \
    lea 1(%%rcx), %%rcx; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc %%al; \
3:;"

   : "=a" (ci)
   : "a" (ci), "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mul1_c
#define HAVE_ARCH_nn_mul1_c

word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        movq  %%rax, (%%rdi,%%r11,8); \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "c" (c), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_addmul1_c
#define HAVE_ARCH_nn_addmul1_c

word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        addq  %%rax, (%%rdi,%%r11,8); \
        adcq  $0, %%rdx; \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "c" (c), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_submul1_c
#define HAVE_ARCH_nn_submul1_c

word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        subq  %%rax, (%%rdi,%%r11,8); \
        adcq  $0, %%rdx; \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "c" (c), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_shl_c
#define HAVE_ARCH_nn_shl_c

word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
1:; \
        xorq  %%r9, %%r9; \
        movq  (%%rsi,%%rdx,8), %%rax; \
        shldq %%rax, %%r9; \
        salq  %%cl, %%rax; \
        addq  %%r8, %%rax; \
        movq  %%rax, (%%rdi,%%rdx,8); \
        movq  %%r9, %%r8; \
        addq  $1, %%rdx; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "c" (bits), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_shr_c
#define HAVE_ARCH_nn_shr_c

word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        jle   2f; \
        subq  $1, %%rdx; \
1:; \
        xorq  %%r9, %%r9; \
        movq  (%%rsi,%%rdx,8), %%rax; \
        shrdq %%rax, %%r9; \
        shrq  %%cl, %%rax; \
        addq  %%r8, %%rax; \
        movq  %%rax, (%%rdi,%%rdx,8); \
        movq  %%r9, %%r8; \
        subq  $1, %%rdx; \
        jns   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "c" (bits), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_neg_c
#define HAVE_ARCH_nn_neg_c

word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
{
   __asm__ __volatile__ (

       "jrcxz 4f; \
        lea   (%%rsi,%%rcx,8), %%rsi; \
        lea   (%%rdi,%%rcx,8), %%rdi; \
        neg   %%rcx; \
        bt  $0, %%rax; \
        cmc; \
1:; \
        jnc   2f; \
        movq  (%%rsi,%%rcx,8), %%rdx; \
        notq  %%rdx; \
        adc   $0, %%rdx; \
        movq  %%rdx, (%%rdi,%%rcx,8); \
        leaq  1(%%rcx), %%rcx; \
        jrcxz 3f; \
        jmp   1b; \
2:; \
        movq  (%%rsi,%%rcx,8), %%rdx; \
        notq  %%rdx; \
        movq  %%rdx, (%%rdi,%%rcx,8); \
        leaq  1(%%rcx), %%rcx; \
        jrcxz 3f; \
        jmp   2b; \
3:; \
        cmc; \
        setc %%al; \
4:;"


   : "=a" (ci)
   : "a" (ci), "c" (m), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifdef __cplusplus
 }
#endif
