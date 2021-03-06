|*
|****************************************************************
|*								*
|*			sidcmp.s				*
|*								*
|*	Compara??o de identificadores				*
|*								*
|*	Vers?o	1.0.0, de 17.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Rotinas:						*
|*		sidcmp						*
|*								*
|*	M?dulo: sidcmp						*
|*		libc/sid					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright ? 2000 NCE/UFRJ - tecle "man licen?a"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Compara dois Identificadores				*
|****************************************************************
|*
|*	int
|*	sidcmp (sid1, sid2)
|*	idp_t	sid1, sid2;
|*
	.global	_sidcmp

_sidcmp:
	movl	4(sp),a0	|* sid1
	movl	8(sp),a1	|* sid2

	cmpml	(a0)+,(a1)+	|* bytes 0 a 3
	jne	1$

	cmpml	(a0)+,(a1)+	|* bytes 4 a 7
	jne	1$

	clrl	d0		|* sid1 == sid2
	rts
1$:
	jhi	2$

	moveq	#1,d0		|* sid1 > sid2
	rts
2$:
	moveq	#-1,d0		|* sid1 < sid2
	rts
