; Copyright:
; * Antony Vennard 2010. All rights reserved.
; * William Hart 2010. All rights reserved.
; * Brian Gladman, 2010. ALL rights reserved.
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

; About: This code is an assembly implementation of
; Bill's nn_add_mc function for x64 processors, influenced
; by his blog and code in nn.c. Finished with a lot of help
; from the mailing list.

; Assumptions: word_t is uint64_t and 8 bytes wide

; data segment. 
SECTION .data

; code segment. 
SECTION .code


; global symbols
GLOBAL _nn_add_mc
GLOBAL _nn_sub_mc: 


; function code.

_nn_add_mc:

    ; Stack frame for this function

    ; push    rbp
    ; mov     rbp, rsp

    ; arguments to functions. 
    ; arguments are in:
    ; * rdi (a)
    ; * rsi (b)
    ; * rdx (c)
    ; * rcx  (m)
    ; * r8  (ci)

    ; zero rax
    xor      rax, rax

    ; load the address of the ends of the word_t arrays.
    lea rdi, [rdi + rcx*8]
    lea rsi, [rsi + rcx*8]
    lea rdx, [rdx + rcx*8]

    ; rcx is a positive value of m. Negate it.
    neg      rcx

    ; determine if there is a carry.
    sar      r8d, 1

    ; if rcx==0, exit.
    jrcxz    _nn_add_mc_exit

_nn_add_mc_loop:

    ; add the two word_t's for each nn_t together.
    mov     r10, [rsi + rcx*8]
    adc     r10, [rdx + rcx*8]
    
    ; store the result
    mov     [rdi+rcx*8], r10

    ; increase rcx until it is zero, then exit.
    inc     rcx
    jnz     _nn_add_mc_loop 

_nn_add_mc_exit:

    ; set return value (rax)
    SETC      al
    
    ; destroy stack frame
    ; pop     rbp

    ; return
    ret


_nn_sub_mc:

    ; Stack frame for this function

    ; push    rbp
    ; mov     rbp, rsp

    ; arguments to functions. 
    ; arguments are in:
    ; * rdi (a)
    ; * rsi (b)
    ; * rdx (c)
    ; * rcx  (m)
    ; * r8  (ci)

    ; zero rax
    xor      rax, rax

    ; load the address of the ends of the word_t arrays.
    lea rdi, [rdi + rcx*8]
    lea rsi, [rsi + rcx*8]
    lea rdx, [rdx + rcx*8]

    ; rcx is a positive value of m. Negate it.
    neg      rcx

    ; determine if there is a carry.
    sar      r8d, 1

    ; if rcx==0, exit.
    jrcxz    _nn_sub_mc_exit

_nn_sub_mc_loop:

    ; add the two word_t's for each nn_t together.
    mov     r10, [rsi + rcx*8]
    sbb     r10, [rdx + rcx*8]
    
    ; store the result
    mov     [rdi+rcx*8], r10

    ; increase rcx until it is zero, then exit.
    inc     rcx
    jnz     _nn_sub_mc_loop 

_nn_sub_mc_exit:

    ; set return value (rax)
    SETC      al
    
    ; destroy stack frame
    ; pop     rbp

    ; return
    ret

