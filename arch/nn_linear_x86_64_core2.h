/* 
  Copyright (C) 2010, William Hart
  Copyright (C) 2010, Brian Gladman

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

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   ASSERT(ci <= 1);
   
   __asm__ (

   ".intel_syntax noprefix; \
    mov rax, r8; \
    jrcxz 3f; \
    lea rdi, [rdi+rcx*8]; \
    lea rsi, [rsi+rcx*8]; \
    lea rdx, [rdx+rcx*8]; \
    neg rcx; \
    bt  rax, 0; \
1:; \
    mov r10, [rsi+rcx*8]; \
    adc r10, [rdx+rcx*8]; \
    mov [rdi+rcx*8], r10; \
    lea rcx, [rcx+1]; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc al; \
3:;"

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
    jrcxz 3f; \
    lea rdi, [rdi+rcx*8]; \
    lea rsi, [rsi+rcx*8]; \
    lea rdx, [rdx+rcx*8]; \
    neg rcx; \
    bt  rax, 0; \
1:; \
    mov r10, [rsi+rcx*8]; \
    sbb r10, [rdx+rcx*8]; \
    mov [rdi+rcx*8], r10; \
    lea rcx, [rcx+1]; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc al; \
3:;"

   : "=a" ((word_t)(bi))
   : "c" ((len_t)(m)), "d" ((nn_src_t)(c)), "S" ((nn_src_t)(b)), "D" ((nn_src_t *)(a))
   );

   return bi;
}

#endif
