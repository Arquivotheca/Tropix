|*
|****************************************************************
|*								*
|*			idcmp.s					*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 02.07.95				*
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
|*	Compara identificadores					*
|****************************************************************
|*
|*	int	idcmp (const idp_t id1, const idp_t id2);
|*
	.global	_idcmp
_idcmp:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "id1"
	movl	16(sp),r4	|* "id2"
	movl	#14,r1		|* 14 bytes
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
