|*
|****************************************************************
|*								*
|*			idhash.s				*
|*								*
|*	Calcula um número hash para um identificador		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Módulo: idhash						*
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
|*	Calcula um Numero Hash para um Identificador		*
|****************************************************************
|*
|*	int
|*	idhash (id, size)
|*	idp_t	id;
|*	ushort	size;
|*
	.global	_idhash

_idhash:
	movl	4(sp),a0	|* id1

	movl	(a0)+,d0	|* Acha o OUtorio dos 4 longs
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0)+,d1
	eorl	d1,d0
	movl	(a0),d1
	eorl	d1,d0

	movl	d0,d1		|* Acha o OUtorio dos 2 shorts
	swap	d0
	eorl	d1,d0
	clrw	d0
	swap	d0

	divu	10(sp),d0	|* Acha o Resto da Divisao
	clrw	d0
	swap	d0

	rts
