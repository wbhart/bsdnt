/* Copyright 2010 (C) Gonzalo Tornaria, Antony Vennard, Brian Gladman, William Hart */

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "movq %%r8, %%rax; \
    jrcxz 3f; \
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
    jrcxz 3f; \
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
   : "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif
