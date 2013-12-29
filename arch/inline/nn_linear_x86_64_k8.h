/* Copyright 2010 (C) Antony Vennard, Brian Gladman, William Hart */

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

#ifdef __cplusplus
 extern "C" {
#endif

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "movq %%r8, %%rax; \
    jrcxz 2f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    sar $1, %%eax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    adc (%%rdx,%%rcx,8), %%r10; \
    mov %%r10, (%%rdi,%%rcx,8); \
    inc %%rcx; \
    jnz 1b; \
    setc %%al; \
2:; "

   : "=a" (ci)
   : "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc
#define HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "mov %%r8, %%rax; \
    jrcxz 2f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    sar $1, %%eax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    sbb (%%rdx,%%rcx,8), %%r10; \
    movq %%r10, (%%rdi,%%rcx,8); \
    incq %%rcx; \
    jnz 1b; \
    setc %%al; \
2:; "

   : "=a" (ci)
   : "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#ifdef __cplusplus
 }
#endif

#endif
