|*
|****************************************************************
|*								*
|*			idclr.s					*
|*								*
|*	Zera um identificador					*
|*								*
|*	Versão	3.0.0, de 14.01.95				*
|*		3.0.0, de 14.01.95				*
|*								*
|*	Módulo: idclr						*
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
|*	Zera um identificador					*
|****************************************************************
|*
|*	idp_t	idclr (idp_t id1);
|*
	.global	_idclr
_idclr:
	pushl	r5

	movl	8(sp),r5	|* Zera os 16 bytes de "id1"
	movl	#4,r1
	clrl	r0

	up
	rep
	stosl

	movl	8(sp),r0	|* Retorna "id1"
	popl	r5
	rts
