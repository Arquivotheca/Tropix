|*
|****************************************************************
|*								*
|*			memsetw.s				*
|*								*
|*	Preenche uma área com um "short"			*
|*								*
|*	Versão	3.0.0, de 24.03.95				*
|*		3.0.0, de 24.03.95				*
|*								*
|*	Modulo: memsetw						*
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
|*	Preenche uma área com um "short"			*
|****************************************************************
|*
|*	void	*memsetw (void *dst, short w, int short_count);
|*
	.global	_memsetw
_memsetw:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r0	|* "w"
	movl	16(fp),r1	|* "short_count"

	up
	rep
	stosw

	movl	8(fp),r0	|* Retorna "dst"

	popl	r5
	unlk
	rts
