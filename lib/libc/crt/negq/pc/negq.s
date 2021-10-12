|*
|****************************************************************
|*								*
|*			negq.s					*
|*								*
|*	Troca de sinal de 64 bits				*
|*								*
|*	Versão	4.4.0, de 31.10.02				*
|*		4.4.0, de 31.10.02				*
|*								*
|*	Módulo: negq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Troca de sinal de 64 bits *******************************
|*
|*	long long ___negdi2 (long long a);
|*
	.text
	.global	___negdi2
___negdi2:
	movl	4(sp),r0
	movl	8(sp),r2

	notl	r0
	notl	r2

	addl	#1,r0
	adcl	#0,r2

	rts
