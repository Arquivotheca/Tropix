|*
|****************************************************************
|*								*
|*			sideq.s					*
|*								*
|*	Verifica a igualdade de dois identificadores		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.02.95				*
|*								*
|*	Módulo: sideq						*
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
|*	int	sideq (const idp_t sid1, const idp_t sid2);
|*
	.global	_sideq
_sideq:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "sid1"
	movl	16(sp),r4	|* "sid2"
	movl	#2,r1		|* 2 longos
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
