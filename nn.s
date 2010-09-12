	.file	"nn.c"
.globl __randval
	.data
	.align 8
	.type	__randval, @object
	.size	__randval, 8
__randval:
	.quad	4035456057
.globl __randval2
	.align 8
	.type	__randval2, @object
	.size	__randval2, 8
__randval2:
	.quad	6748392731
	.text
.globl randinit
	.type	randinit, @function
randinit:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	randinit, .-randinit
.globl randclear
	.type	randclear, @function
randclear:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	randclear, .-randclear
.globl randword
	.type	randword, @function
randword:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%rbx
	movq	%rdi, -16(%rbp)
	movq	__randval(%rip), %rax
	imulq	$1025416097, %rax, %rax
	leaq	286824428(%rax), %rcx
	movabsq	$-64424509215, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	shrq	$32, %rdx
	movq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rbx
	.cfi_offset 3, -24
	salq	$28, %rbx
	addq	%rbx, %rax
	subq	%rdx, %rax
	movq	%rcx, %rdx
	subq	%rax, %rdx
	movq	%rdx, __randval(%rip)
	movq	__randval2(%rip), %rax
	imulq	$1647637699, %rax, %rax
	leaq	286824428(%rax), %rcx
	movabsq	$-261993001335, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$32, %rax
	movabsq	$4294967357, %rdx
	imulq	%rdx, %rax
	movq	%rcx, %rdx
	subq	%rax, %rdx
	movq	%rdx, %rax
	movq	%rax, __randval2(%rip)
	movq	__randval2(%rip), %rax
	movq	%rax, %rdx
	salq	$32, %rdx
	movq	__randval(%rip), %rax
	leaq	(%rdx,%rax), %rax
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	randword, .-randword
.globl randint
	.type	randint, @function
randint:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	randword
	movl	$0, %edx
	divq	-8(%rbp)
	movq	%rdx, %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	randint, .-randint
.globl nn_random
	.type	nn_random, @function
nn_random:
.LFB15:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$40, %rsp
	movq	%rdi, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	%rdx, -48(%rbp)
	movq	$0, -16(%rbp)
	jmp	.L10
	.cfi_offset 3, -24
.L11:
	movq	-16(%rbp), %rax
	salq	$3, %rax
	movq	%rax, %rbx
	addq	-32(%rbp), %rbx
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	randword
	movq	%rax, (%rbx)
	addq	$1, -16(%rbp)
.L10:
	movq	-16(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jl	.L11
	addq	$40, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	nn_random, .-nn_random
.globl _nn_add_mc
	.type	_nn_add_mc, @function
_nn_add_mc:
.LFB16:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movq	%rcx, -96(%rbp)
	movq	%r8, -104(%rbp)
	movq	$0, -40(%rbp)
	jmp	.L14
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
.L15:
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-80(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r13
	movl	$0, %r14d
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r11
	movl	$0, %r12d
	movq	%r13, %rax
	movq	%r14, %rdx
	addq	%r11, %rax
	adcq	%r12, %rdx
	movq	-104(%rbp), %r9
	movl	$0, %r10d
	addq	%r9, %rax
	adcq	%r10, %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-72(%rbp), %rax
	movq	-64(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-64(%rbp), %rax
	movq	-56(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, -104(%rbp)
	addq	$1, -40(%rbp)
.L14:
	movq	-40(%rbp), %rax
	cmpq	-96(%rbp), %rax
	jl	.L15
	movq	-104(%rbp), %rax
	popq	%r12
	popq	%r13
	popq	%r14
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	_nn_add_mc, .-_nn_add_mc
.globl _nn_sub_mc
	.type	_nn_sub_mc, @function
_nn_sub_mc:
.LFB17:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movq	%rcx, -96(%rbp)
	movq	%r8, -104(%rbp)
	movq	$0, -40(%rbp)
	jmp	.L18
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
.L19:
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-80(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r9
	movl	$0, %r10d
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r13
	movl	$0, %r14d
	movq	%r9, %rax
	movq	%r10, %rdx
	subq	%r13, %rax
	sbbq	%r14, %rdx
	movq	-104(%rbp), %r11
	movl	$0, %r12d
	subq	%r11, %rax
	sbbq	%r12, %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-72(%rbp), %rax
	movq	-64(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-64(%rbp), %rax
	movq	-56(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	negq	%rax
	movq	%rax, -104(%rbp)
	addq	$1, -40(%rbp)
.L18:
	movq	-40(%rbp), %rax
	cmpq	-96(%rbp), %rax
	jl	.L19
	movq	-104(%rbp), %rax
	popq	%r12
	popq	%r13
	popq	%r14
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	_nn_sub_mc, .-_nn_sub_mc
.globl _nn_shl_c
	.type	_nn_shl_c, @function
_nn_shl_c:
.LFB18:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movq	%r8, -72(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L22
.L23:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-48(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r9
	movl	$0, %r10d
	movq	-64(%rbp), %rax
	movl	%eax, %ecx
	movq	%r9, %rax
	movq	%r10, %rdx
	shldq	%rax, %rdx
	salq	%cl, %rax
	testb	$64, %cl
	je	.L25
	movq	%rax, %rdx
	xorl	%eax, %eax
.L25:
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-32(%rbp), %rdx
	addq	-72(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, -72(%rbp)
	addq	$1, -8(%rbp)
.L22:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jl	.L23
	movq	-72(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	_nn_shl_c, .-_nn_shl_c
.globl _nn_shr_c
	.type	_nn_shr_c, @function
_nn_shr_c:
.LFB19:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movq	%r8, -72(%rbp)
	movl	$64, %eax
	subq	-64(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-56(%rbp), %rax
	subq	$1, %rax
	movq	%rax, -8(%rbp)
	jmp	.L27
.L28:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-48(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r9
	movl	$0, %r10d
	movq	-64(%rbp), %rax
	movl	%eax, %ecx
	movq	%r9, %rax
	movq	%r10, %rdx
	shldq	%rax, %rdx
	salq	%cl, %rax
	testb	$64, %cl
	je	.L30
	movq	%rax, %rdx
	xorl	%eax, %eax
.L30:
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	salq	$3, %rax
	movq	%rax, %rcx
	addq	-40(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	addq	-72(%rbp), %rax
	movq	%rax, (%rcx)
	movq	-32(%rbp), %rax
	movq	%rax, -72(%rbp)
	subq	$1, -8(%rbp)
.L27:
	cmpq	$0, -8(%rbp)
	jns	.L28
	movq	-72(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	_nn_shr_c, .-_nn_shr_c
.globl _nn_mul1_c
	.type	_nn_mul1_c, @function
_nn_mul1_c:
.LFB20:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movq	%rcx, -96(%rbp)
	movq	%r8, -104(%rbp)
	movq	$0, -40(%rbp)
	jmp	.L32
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
.L33:
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-80(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r11
	movl	$0, %r12d
	movq	-96(%rbp), %r9
	movl	$0, %r10d
	movq	%r12, %rcx
	imulq	%r9, %rcx
	movq	%r10, %rax
	imulq	%r11, %rax
	addq	%rax, %rcx
	movq	%r11, %rax
	mulq	%r9
	movq	%rax, -128(%rbp)
	movq	%rdx, -120(%rbp)
	addq	-120(%rbp), %rcx
	movq	%rcx, -120(%rbp)
	movq	-104(%rbp), %r13
	movl	$0, %r14d
	movq	-128(%rbp), %rax
	movq	-120(%rbp), %rdx
	addq	%r13, %rax
	adcq	%r14, %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-40(%rbp), %rax
	salq	$3, %rax
	addq	-72(%rbp), %rax
	movq	-64(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-64(%rbp), %rax
	movq	-56(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, -104(%rbp)
	addq	$1, -40(%rbp)
.L32:
	movq	-40(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jl	.L33
	movq	-104(%rbp), %rax
	popq	%r12
	popq	%r13
	popq	%r14
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	_nn_mul1_c, .-_nn_mul1_c
.globl _nn_add1
	.type	_nn_add1, @function
_nn_add1:
.LFB21:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L36
.L38:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-48(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r10
	movl	$0, %r11d
	movq	-64(%rbp), %r8
	movl	$0, %r9d
	movq	%r10, %rax
	movq	%r11, %rdx
	addq	%r8, %rax
	adcq	%r9, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, -64(%rbp)
	addq	$1, -8(%rbp)
.L36:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jge	.L37
	cmpq	$0, -64(%rbp)
	jne	.L38
.L37:
	movq	-40(%rbp), %rax
	cmpq	-48(%rbp), %rax
	je	.L39
	jmp	.L40
.L41:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-8(%rbp), %rdx
	salq	$3, %rdx
	addq	-48(%rbp), %rdx
	movq	(%rdx), %rdx
	movq	%rdx, (%rax)
	addq	$1, -8(%rbp)
.L40:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jl	.L41
.L39:
	movq	-64(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	_nn_add1, .-_nn_add1
.globl _nn_sub1
	.type	_nn_sub1, @function
_nn_sub1:
.LFB22:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L44
.L46:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-48(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %r8
	movl	$0, %r9d
	movq	-64(%rbp), %r10
	movl	$0, %r11d
	movq	%r8, %rax
	movq	%r9, %rdx
	subq	%r10, %rax
	sbbq	%r11, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	negq	%rax
	movq	%rax, -64(%rbp)
	addq	$1, -8(%rbp)
.L44:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jge	.L45
	cmpq	$0, -64(%rbp)
	jne	.L46
.L45:
	movq	-40(%rbp), %rax
	cmpq	-48(%rbp), %rax
	je	.L47
	jmp	.L48
.L49:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-8(%rbp), %rdx
	salq	$3, %rdx
	addq	-48(%rbp), %rdx
	movq	(%rdx), %rdx
	movq	%rdx, (%rax)
	addq	$1, -8(%rbp)
.L48:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jl	.L49
.L47:
	movq	-64(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	_nn_sub1, .-_nn_sub1
.globl _nn_neg_c
	.type	_nn_neg_c, @function
_nn_neg_c:
.LFB23:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	%rcx, -64(%rbp)
	movl	$1, %eax
	subq	-64(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L52
.L54:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-48(%rbp), %rax
	movq	(%rax), %rax
	notq	%rax
	movq	%rax, %r10
	movl	$0, %r11d
	movq	-64(%rbp), %r8
	movl	$0, %r9d
	movq	%r10, %rax
	movq	%r11, %rdx
	addq	%r8, %rax
	adcq	%r9, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, -64(%rbp)
	addq	$1, -8(%rbp)
.L52:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jge	.L55
	cmpq	$0, -64(%rbp)
	jne	.L54
	jmp	.L55
.L56:
	movq	-8(%rbp), %rax
	salq	$3, %rax
	addq	-40(%rbp), %rax
	movq	-8(%rbp), %rdx
	salq	$3, %rdx
	addq	-48(%rbp), %rdx
	movq	(%rdx), %rdx
	notq	%rdx
	movq	%rdx, (%rax)
	addq	$1, -8(%rbp)
.L55:
	movq	-8(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jl	.L56
	movl	$1, %eax
	subq	-64(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	_nn_neg_c, .-_nn_neg_c
.globl nn_cmp_m
	.type	nn_cmp_m, @function
nn_cmp_m:
.LFB24:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	jmp	.L59
.L62:
	movq	-24(%rbp), %rax
	salq	$3, %rax
	addq	-8(%rbp), %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	salq	$3, %rax
	addq	-16(%rbp), %rax
	movq	(%rax), %rax
	cmpq	%rax, %rdx
	je	.L59
	movq	-24(%rbp), %rax
	salq	$3, %rax
	addq	-8(%rbp), %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	salq	$3, %rax
	addq	-16(%rbp), %rax
	movq	(%rax), %rax
	cmpq	%rax, %rdx
	jbe	.L60
	movl	$1, %eax
	jmp	.L61
.L60:
	movl	$-1, %eax
	jmp	.L61
.L59:
	cmpq	$0, -24(%rbp)
	setne	%al
	subq	$1, -24(%rbp)
	testb	%al, %al
	jne	.L62
	movl	$0, %eax
.L61:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE24:
	.size	nn_cmp_m, .-nn_cmp_m
	.ident	"GCC: (GNU) 4.4.4 20100630 (Red Hat 4.4.4-10)"
	.section	.note.GNU-stack,"",@progbits
