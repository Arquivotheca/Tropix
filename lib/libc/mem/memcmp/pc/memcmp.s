|*
|****************************************************************
|*								*
|*			memcmp.s				*
|*								*
|*	Compara áreas de memoria				*
|*								*
|*	Versão	3.0.0, de 07.04.95				*
|*		3.0.0, de 14.04.95				*
|*								*
|*	Módulo: memcmp						*
|*		libc/mem					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Compara áreas de memoria				*
|****************************************************************
|*
|*	int	memcmp (const void *dst, const void *src, size_t count);
|*
	.global	_memcmp
_memcmp:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	clrl	r0		|* Por enquanto, é igual

	up			|* De baixo para cima 
	repe
	cmpsb

	jeq	2$		|* Se foi igual, ...
	jhi	1$		|* Se foi maior, ...

	incl	r0		|* "dst" > "src"
	jmp	2$
1$:
	decl	r0		|* "dst" < "src"
2$:
	popl	r4
	popl	r5
	unlk
	rts
