|*
|****************************************************************
|*								*
|*			lideq.s					*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.02.95				*
|*								*
|*	Módulo: lideq						*
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
|*	int	lideq (const idp_t lid1, const idp_t lid2);
|*
	.global	_lideq
_lideq:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "lid1"
	movl	16(sp),r4	|* "lid2"
	movl	#8,r1		|* 8 longos
	clrl	r0		|* Por enquanto o resultado é NÃO

	up
	repe
	cmpsl
	jne	1$

	incl	r0		|* Retorna "1"
1$:
	popl	r4
	popl	r5
	rts
