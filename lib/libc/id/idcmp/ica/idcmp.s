|*
|****************************************************************
|*								*
|*			idcmp.s					*
|*								*
|*	Comparação de identificadores				*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Rotinas:						*
|*		idcmp						*
|*								*
|*	Módulo: idcmp						*
|*		libc/id						*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Compara dois Identificadores				*
|****************************************************************
|*
|*	int
|*	idcmp (id1, id2)
|*	idp_t	id1, id2;
|*
	.global	_idcmp

_idcmp:
	movl	4(sp),a0	|* id1
	movl	8(sp),a1	|* id2

	cmpml	(a0)+,(a1)+	|* bytes 0 a 3
	jne	1$

	cmpml	(a0)+,(a1)+	|* bytes 4 a 7
	jne	1$

	cmpml	(a0)+,(a1)+	|* bytes 8 a 11
	jne	1$

	cmpmw	(a0)+,(a1)+	|* bytes 12 a 13
	jne	1$

	clrl	d0		|* id1 == id2
	rts
1$:
	jhi	2$

	moveq	#1,d0		|* id1 > id2
	rts
2$:
	moveq	#-1,d0		|* id1 < id2
	rts
