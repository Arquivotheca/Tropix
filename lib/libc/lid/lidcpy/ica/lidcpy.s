|*
|****************************************************************
|*								*
|*			lidcpy.s				*
|*								*
|*	Cópia de identificadores longos				*
|*								*
|*	Versão	2.1.0, de 20.01.89				*
|*								*
|*	Módulo: lidcpy						*
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
|*	Copia um Identificador longo				*
|****************************************************************
|*
|*	idp_t
|*	lidcpy (lid1, lid2)
|*	idp_t	lid1, lid2;
|*
	.global	_lidcpy

_lidcpy:
	movl	4(sp),a0	|* lid1
	movl	a0,d0
	movl	8(sp),a1	|* lid2

	movl	(a1)+,(a0)+	|* Copia os 32 bytes
	movl	(a1)+,(a0)+
	movl	(a1)+,(a0)+
	movl	(a1)+,(a0)+

	movl	(a1)+,(a0)+
	movl	(a1)+,(a0)+
	movl	(a1)+,(a0)+
	movl	(a1),(a0)

	rts
