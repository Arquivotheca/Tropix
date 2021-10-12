|*
|****************************************************************
|*								*
|*			memsetl.s				*
|*								*
|*	Preenche uma área com um longo				*
|*								*
|*	Versão	3.0.0, de 29.01.95				*
|*		3.0.0, de 29.01.95				*
|*								*
|*	Modulo: memsetl						*
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
|*	Preenche uma área com um longo				*
|****************************************************************
|*
|*	void	*memsetl (void *dst, long l, int long_count);
|*
	.global	_memsetl
_memsetl:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r0	|* "l"
	movl	16(fp),r1	|* "long_count"

	up
	rep
	stosl

	movl	8(fp),r0	|* Retorna "dst"

	popl	r5
	unlk
	rts
