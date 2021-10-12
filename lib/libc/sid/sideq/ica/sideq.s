|*
|****************************************************************
|*								*
|*			sideq.s					*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 17.04.89				*
|*								*
|*	Rotinas:						*
|*		sideq						*
|*								*
|*	Módulo: sideq						*
|*		libc/sid					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Testa a Igualdade de dois Identificadores		*
|****************************************************************
|*
|*	int
|*	sideq (sid1, sid2)
|*	idp_t	sid1, sid2;
|*
	.global	_sideq

_sideq:
	movl	4(sp),a0	|* sid1
	movl	8(sp),a1	|* sid2

	cmpml	(a0)+,(a1)+	|* bytes 0 a 3
	jeq	2$
1$:
	clrl	d0		|* Sao diferentes
	rts
2$:
	cmpml	(a0)+,(a1)+	|* bytes 4 a 7
	jne	1$

	moveq	#1,d0		|* sid1 == sid2
	rts
