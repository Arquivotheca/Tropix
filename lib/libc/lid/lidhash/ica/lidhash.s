|*
|****************************************************************
|*								*
|*			lidhash.s				*
|*								*
|*	Calcula um número hash para um identificador longo	*
|*								*
|*	Versão	2.1.0, de 20.20.89				*
|*								*
|*	Módulo: lidhash						*
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
|*	Calcula um Numero Hash para um Identificador longo	*
|****************************************************************
|*
|*	int
|*	lidhash (lid, size)
|*	idp_t	lid;
|*	ushort	size;
|*
	.global	_lidhash

_lidhash:
	movl	4(sp),a0	|* lid1

	movl	(a0)+,d0	|* Acha o OUtorio dos longos 0 a 3
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0)+,d1
	eorl	d1,d0

	movl	(a0)+,d1	|* Acha o OUtorio dos longos 4 a 7
	jeq	1$
	eorl	d1,d0
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0),d1
	eorl	d1,d0
1$:
	movl	d0,d1		|* Acha o OUtorio dos 2 shorts
	swap	d0
	eorl	d1,d0
	clrw	d0
	swap	d0

	divu	10(sp),d0	|* Acha o Resto da Divisao
	clrw	d0
	swap	d0

	rts
