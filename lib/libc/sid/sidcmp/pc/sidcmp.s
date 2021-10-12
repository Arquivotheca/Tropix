|*
|****************************************************************
|*								*
|*			sidcmp.s				*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 04.07.95				*
|*								*
|*	Módulo: sidcmp						*
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
|*	Compara identificadores					*
|****************************************************************
|*
|*	int	sidcmp (const idp_t sid1, const idp_t sid2);
|*
	.global	_sidcmp
_sidcmp:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "sid1"
	movl	16(sp),r4	|* "sid2"
	movl	#7,r1		|* 7 bytes
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
