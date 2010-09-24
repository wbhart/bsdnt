/* Copyright 2010 (C) Antony Vennard, Brian Gladman, William Hart */

#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
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

