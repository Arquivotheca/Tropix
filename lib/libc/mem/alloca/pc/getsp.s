	.global	_getsp

_getsp:
	movl	sp,r0
	addl	#4,r0
	rts
