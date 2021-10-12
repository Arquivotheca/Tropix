	.text
start0:
	jmp	beta0

	.align	4
alpha0:
	movl	#3,sp

|*	.align	4
gamma0:
	.double	1.0

	.dsb	20

beta0:
	movl	#3,sp
	


	.org	start0+512
start1:
	jmp	beta1


	.align	4
alpha1:
	movl	#3,sp

gamma1:
	.double	1.0

	.dsb	200

beta1:
	movl	#3,sp
