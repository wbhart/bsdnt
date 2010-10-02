/* Copyright 2010 (C) Antony Vennard, Brian Gladman, William Hart */

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   ASSERT(ci <= 1);
   
   __asm__ (

   ".intel_syntax noprefix; \
    mov rax, r8; \
    jrcxz 2f; \
    lea rdi, [rdi+rcx*8]; \
    lea rsi, [rsi+rcx*8]; \
    lea rdx, [rdx+rcx*8]; \
    neg rcx; \
    sar eax, 1; \
1:; \
    mov r10, [rsi+rcx*8]; \
    adc r10, [rdx+rcx*8]; \
    mov [rdi+rcx*8], r10; \
    inc rcx; \
    jnz 1b; \
    setc al; \
2:; "

   : "=a" ((word_t)(ci))
   : "c" ((len_t)(m)), "d" ((nn_src_t)(c)), "S" ((nn_src_t)(b)), "D" ((nn_src_t *)(a))
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc
#define HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi)
{
   ASSERT(bi <= 1);
   
   __asm__ (

   ".intel_syntax noprefix; \
    mov rax, r8; \
    jrcxz 2f; \
    lea rdi, [rdi+rcx*8]; \
    lea rsi, [rsi+rcx*8]; \
    lea rdx, [rdx+rcx*8]; \
    neg rcx; \
    sar eax, 1; \
1:; \
    mov r10, [rsi+rcx*8]; \
    sbb r10, [rdx+rcx*8]; \
    mov [rdi+rcx*8], r10; \
    inc rcx; \
    jnz 1b; \
    setc al; \
2:; "

   : "=a" ((word_t)(bi))
   : "c" ((len_t)(m)), "d" ((nn_src_t)(c)), "S" ((nn_src_t)(b)), "D" ((nn_src_t *)(a))
   );

   return bi;
}

#endif
