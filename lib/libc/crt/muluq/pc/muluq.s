|*
|****************************************************************
|*								*
|*			muluq.s					*
|*								*
|*	Multiplicação de 64 bits sem sinal			*
|*								*
|*	Versão	4.4.0, de 29.10.02				*
|*		4.4.0, de 29.10.02				*
|*								*
|*	Módulo: muluq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Multiplicação 64 bits sem sinal *************************
|*
	.global	___muldi3
___muldi3:
	movl	8(sp),r0
	orl	16(sp),r0
	jnz	1$

	movl	4(sp),r0
	mulul	12(sp),r0
	rts
1$:
	movl	16(sp),r0
	mulul	4(sp),r0
	movl	r0,r1

	movl	8(sp),r0
	mulul	12(sp),r0
	addl	r0,r1

	movl	4(sp),r0
	mulul	12(sp),r0
	addl	r1,r2

	rts
