; Copyright Antony Vennard 2010. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without 
; modification, are permitted provided that the following conditions are 
; met:
;
;    1. Redistributions of source code must retain the above copyright 
;       notice, this list of conditions and the following disclaimer.
; 
;    2. Redistributions in binary form must reproduce the above copyright 
;       notice, this list of conditions and the following disclaimer in the 
;       documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY Antony Vennard ``AS IS'' AND ANY EXPRESS OR 
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
; NO EVENT SHALL Antony Vennard OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
; OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
; DAMAGE.
;
; The views and conclusions contained in the software and documentation are 
; those of the authors and should not be interpreted as representing official 
; policies, either expressed or implied, of Antony Vennard.
;
; =============================================================================

; data segment. 
SECTION .data

    WORDSIZE dq 8

; code segment. 
SECTION .code


; global symbols
GLOBAL _nn_add_mc


; function code.

_nn_add_mc:

    ; Stack frame for this function

    push    rbp
    mov     rbp, rsp

    ; get arguments to functions. We expect an integer type
    ; or rather a pointer to be passed to an integer type
    ; we don't think it will be SSE (xmm0-7) or X87.

    ; move into gen purpose regs.
    mov     rax, rdi            ; nn_t a (ptr)
    mov     r11, rsi            ; nn_t b (ptr) 

    ; these calls are technically redundant. Left
    ; for now, for clarity.
    mov     rdx, rdx            ; nn_t c (ptr)
    mov     r8, r8              ; len_t m 
    mov     r9, r9              ; word_t ci 

    mov     rcx, 0

_nn_add_mc_loop:

    ; t = 0
    xor     r10,r10 

    ; t = (dword_t) b[i] + (dword_t) c[i] + (dword_t) ci;

    add     r10, [r11 + rcx*WORDSIZE]
    add     r10, [rdx + rcx*WORDSIZE]
    add     r10, [r9  + rcx*WORDSIZE]
    
    mov     r8, r10
    mov     r9, r10
    shl     r8, WORDSIZE

    ; a[i] = (word_t) t; => a[i] = LOWORD
    mov     result, r8;

    ; ci = (t >> WORD_BITS); a[i] = HIWORD
    shr     r9, WORDSIZE

    ; loop control
    inc     rcx
    cmp     r15, rcx


_nn_add_mc_exit:

    ; set return value (rax)
    mov     rax, r9             ; return carry

    ; destroy stack frame
    pop     rbp

    ; return
    ret



