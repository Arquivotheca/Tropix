	.const
LC0:
	.isoz	"%d, %d, %d, %d, %d\n"
	.text
	.global	_main
_main:
	link	#16
	pushl	r5
	pushl	r4
	pushl	r3
	movl	12(fp),r4
	cmpl	#4,8(fp)
	jeq	.L2
	clrl	r0
	jmp	.L1
.L2:
	pushl	#0
	leal	-4(fp),r3
	pushl	r3
	pushl	4(r4)
	call	_strtol
	movl	r0,-16(fp)
	pushl	#0
	pushl	r3
	pushl	8(r4)
	call	_strtol
	movl	r0,r5
	pushl	#0
	pushl	r3
	pushl	12(r4)
	call	_strtol
	movl	r0,r4
	leal	-12(fp),r3
	movl	r5,(r3)
	movl	-16(fp),r1
	movl	r1,4(r3)
	clrl	r2
	movl	r4,r0
	tstl	r4
	jge	.L3
	notl	r2
.L3:
	pushl	r2
	pushl	r0
	pushl	-8(fp)
	pushl	-12(fp)
	call	___udivdi3
	addl	#16,sp
	movl	r0,-12(fp)
	movl	r2,-8(fp)
	addl	#36,sp
	pushl	(r3)
	pushl	4(r3)
	pushl	r4
	pushl	r5
	pushl	-16(fp)
	pushl	#LC0
	call	_printf
.L1:
	leal	-28(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
