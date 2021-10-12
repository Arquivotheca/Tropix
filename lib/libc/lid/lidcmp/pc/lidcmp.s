|*
|****************************************************************
|*								*
|*			lidcmp.s				*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.07.95				*
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
|*	Compara identificadores					*
|****************************************************************
|*
|*	int	lidcmp (const idp_t lid1, const idp_t lid2);
|*
	.global	_lidcmp
_lidcmp:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "lid1"
	movl	16(sp),r4	|* "lid2"
	movl	#31,r1		|* 31 bytes
	clrl	r0		|* Por enquanto o resultado é NÃO

	up
	repe
	cmpsb			|* Infelizmente não dá para usar longs!
	jeq	9$

	jhi	2$

	incl	r0		|* Retorna "1"
	jmp	9$
2$:
	decl	r0		|* Retorna "-1"
9$:
	popl	r4
	popl	r5
	rts
