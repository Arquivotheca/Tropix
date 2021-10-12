|*
|****************************************************************
|*								*
|*			lidhash.s				*
|*								*
|*	Calcula um número hash para um identificador longo	*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.07.95				*
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
|*	Calcula um número hash para um identificador longo	*
|****************************************************************
|*
|*	int
|*	lidhash (idp_t lid, int size)
|*
	.global	_lidhash
_lidhash:
	movl	4(sp),r1	|* Endereço de lid

	movl	 0(r1),r0	|* Outório dos 8 longos 
	eorl	 4(r1),r0
	eorl	 8(r1),r0
	eorl	12(r1),r0

	cmpl	#0,16(r1)
	jeq	9$

	eorl	16(r1),r0
	eorl	20(r1),r0
	eorl	24(r1),r0
	eorl	28(r1),r0

9$:
BIG_ENDIAN = 0
.if 	BIG_ENDIAN
	xchgb	r0,h0		|* Usa "big endian"
	rorl	#16,r0
	xchgb	r0,h0
.endif	BIG_ENDIAN

	clrl	r2
	divul	8(sp),r0
	movl	r2,r0

	rts
