|*
|****************************************************************
|*								*
|*			lidcan.s				*
|*								*
|*	Converte um identificador longo para a forma canônica	*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.02.95				*
|*								*
|*	Módulo: lidcan						*
|*		libc/lid					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Converte um identificador longo para a forma canônica	*
|****************************************************************
|*
|*	idp_t	lidcan (idp_t lid1, const char *id2);
|*
	.global	_lidcan
_lidcan:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* Zera os 32 bytes de "lid1"
	movl	#8,r1
	clrl	r0
	up
	rep
	stosl

	movl	12(sp),r5	|* "lid1"
	movl	r5,r0		|* Retorna "lid1"
	movl	16(sp),r4	|* "lid2"

	movl	#31,r1		|* Examina 31 caracteres
   |***	up
1$:
	cmpb	#0,(r4)
	movsb
	loopnz	1$

	popl	r4
	popl	r5
	rts
