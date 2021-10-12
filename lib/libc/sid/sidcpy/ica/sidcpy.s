|*
|****************************************************************
|*								*
|*			sidcpy.s				*
|*								*
|*	Cópia de identificadores				*
|*								*
|*	Versão	1.0.0, de 27.10.86				*
|*		2.3.0, de 17.04.88				*
|*								*
|*	Módulo: sidcpy						*
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
|*	Copia um Identificador					*
|****************************************************************
|*
|*	idp_t
|*	sidcpy (sid1, sid2)
|*	idp_t	sid1, sid2;
|*
	.global	_sidcpy

_sidcpy:
	movl	4(sp),a0	|* sid1
	movl	a0,d0
	movl	8(sp),a1	|* sid2

	movl	(a1)+,(a0)+	|* Copia os 8 bytes
	movl	(a1),(a0)

	rts
