|*
|****************************************************************
|*								*
|*			memset.s				*
|*								*
|*	Preenche uma área com um caractere			*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Modulo: memset						*
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
|*	Preenche uma área com um caractere			*
|****************************************************************
|*
|*	void	*memset (void *dst, char c, int count);
|*
	.global	_memset
_memset:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movb	12(fp),r0	|* "c"
	movl	16(fp),r1	|* "count"

	up
	rep
	stosb

	movl	8(fp),r0	|* Retorna "dst"

	popl	r5
	unlk
	rts
