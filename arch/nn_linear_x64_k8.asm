
;  Copyright (C) 2010, William Hart
;  Copyright (C) 2010, Antony Vennard
;  Copyright (C) 2010, Gonzalo Tornaria
;  Copyright (C) 2010, Brian Gladman
;
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
;
;  1. Redistributions of source code must retain the above copyright notice, 
;     this list of conditions and the following disclaimer.
;
;  2. Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;	 documentation and/or other materials provided with the distribution.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
;  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

IFNDEF  CPP
sym_add	EQU	nn_add_mc
sym_sub	EQU	nn_sub_mc
ELSE
sym_add EQU ?nn_add_mc@@YA_KPEA_KPEB_K1J_K@Z
sym_sub EQU ?nn_sub_mc@@YA_KPEA_KPEB_K1J_K@Z
ENDIF

	.CODE
	PUBLIC      sym_add
	PUBLIC      sym_sub

; word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
; rax                 rcx         rdx          r8       r9   [rsp+40]

	ALIGN	16
sym_add		PROC
	mov		r10, rcx
	mov		rcx, r9
	mov		rax, [rsp+40]

    jrcxz	L2
    lea		r10, [r10+rcx*8]
    lea		rdx, [rdx+rcx*8]
    lea		r8, [r8+rcx*8]
    neg		rcx
    sar		eax, 1
L1:	mov		r9, [rdx+rcx*8]
    adc		r9, [r8+rcx*8]
    mov		[r10+rcx*8], r9
    inc		rcx
    jnz		L1
    setc	al
L2:	ret
sym_add		ENDP

; word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
; rax                 rcx         rdx          r8       r9   [rsp+40]

	ALIGN	16
sym_sub	PROC
	mov		r10, rcx
	mov		rcx, r9
	mov		rax, [rsp+40]

    jrcxz	M2
    lea		r10, [r10+rcx*8]
    lea		rdx, [rdx+rcx*8]
    lea		r8, [r8+rcx*8]
    neg		rcx
    sar		eax, 1
M1:	mov		r9, [rdx+rcx*8]
    sbb		r9, [r8+rcx*8]
    mov		[r10+rcx*8], r9
    inc		rcx
    jnz		M1
    setc	al
M2:	ret
sym_sub     ENDP

	END
