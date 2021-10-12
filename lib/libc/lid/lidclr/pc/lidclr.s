|*
|****************************************************************
|*								*
|*			lidclr.s				*
|*								*
|*	Zera um identificador longo				*
|*								*
|*	Versão	3.0.0, de 14.01.95				*
|*		3.0.0, de 14.01.95				*
|*								*
|*	Módulo: lidclr						*
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
|*	Zera um identificador longo				*
|****************************************************************
|*
|*	idp_t	lidclr (idp_t id1);
|*
	.global	_lidclr
_lidclr:
	pushl	r5

	movl	8(sp),r5	|* Zera os 32 bytes de "lid1"
	movl	#8,r1
	clrl	r0

	up
	rep
	stosl

	movl	8(sp),r0	|* Retorna "lid1"
	popl	r5
	rts
