|*
|****************************************************************
|*								*
|*			ideq.s					*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: ideq						*
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
|*	int	ideq (const idp_t id1, const idp_t id2);
|*
	.global	_ideq
_ideq:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "id1"
	movl	16(sp),r4	|* "id2"
	movl	#3,r1		|* 3 longos + 1 short
	clrl	r0		|* Por enquanto o resultado é NÃO

	up
	repe
	cmpsl
	jne	1$

	cmpsw
	jne	1$

	incl	r0		|* Retorna "1"
1$:
	popl	r4
	popl	r5
	rts
