
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

	.CODE
	PUBLIC      div_128_by_64

; word_t div_128_by_64(dword_t *n,  word_t d, word_t *r)
; rax                         rcx,       rdx,        r8

	ALIGN	16
div_128_by_64 PROC
	mov		r9, rdx
	mov		rax, [rcx]
	mov		rdx, [rcx + 8]
	div		r9
	mov		[r8], rdx
	ret
div_128_by_64 ENDP

	END
