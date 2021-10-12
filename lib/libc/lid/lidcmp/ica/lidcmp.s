|*
|****************************************************************
|*								*
|*			lidcmp.s				*
|*								*
|*	Comparação de identificadores longos			*
|*								*
|*	Versão	2.1.0, de 20.01.89				*
|*								*
|*	Rotinas:						*
|*		lidcmp						*
|*								*
|*	Módulo: lidcmp						*
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
|*	Compara dois Identificadores longos			*
|****************************************************************
|*
|*	int
|*	lidcmp (lid1, lid2)
|*	idp_t	lid1, lid2;
|*
	.global	_lidcmp

_lidcmp:
	movl	4(sp),a0	|* lid1
	movl	8(sp),a1	|* lid2

|*	Primeira seção: bytes 0 a 15

	cmpml	(a0)+,(a1)+	|* bytes 0 a 3
	jne	2$

	cmpml	(a0)+,(a1)+	|* bytes 4 a 7
	jne	2$

	cmpml	(a0)+,(a1)+	|* bytes 8 a 11
	jne	2$

	movl	(a1)+,d0	|* bytes 12 a 15
	cmpl	(a0)+,d0
	jne	2$

	tstl	d0		|* Intervalo
	jne	1$

	clrl	d0		|* lid1 == lid2
	rts

|*	Segunda seção: bytes 14 a 31

1$:
	cmpml	(a0)+,(a1)+	|* bytes 16 a 19
	jne	2$

	cmpml	(a0)+,(a1)+	|* bytes 20 a 23
	jne	2$

	cmpml	(a0)+,(a1)+	|* bytes 24 a 27
	jne	2$

	cmpml	(a0)+,(a1)+	|* bytes 28 a 31
	jne	2$

	clrl	d0		|* lid1 == lid2
	rts
2$:
	jhi	3$

	moveq	#1,d0		|* lid1 > lid2
	rts
3$:
	moveq	#-1,d0		|* lid1 < lid2
	rts
