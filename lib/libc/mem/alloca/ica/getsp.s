	.global	_getsp
_getsp:
	movl	sp,d0
	addql	#4,d0
	rts
