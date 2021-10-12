|*
|****************************************************************
|*								*
|*			idcpy.s					*
|*								*
|*	Cópia de identificadores				*
|*								*
|*	Versão	1.0.0, de 27.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Módulo: idcpy						*
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
|*	Copia um Identificador					*
|****************************************************************
|*
|*	idp_t
|*	idcpy (id1, id2)
|*	idp_t	id1, id2;
|*
	.global	_idcpy

_idcpy:
	movl	4(sp),a0	|* id1
	movl	a0,d0
	movl	8(sp),a1	|* id2

	movl	(a1)+,(a0)+	|* Copia os 16 bytes
	movl	(a1)+,(a0)+
	movl	(a1)+,(a0)+
	movl	(a1),(a0)

	rts
