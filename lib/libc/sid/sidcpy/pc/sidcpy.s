|*
|****************************************************************
|*								*
|*			sidcpy.s				*
|*								*
|*	Copia identificadores					*
|*								*
|*	Versão	1.0.0, de 27.10.86				*
|*		3.0.0, de 09.01.95				*
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
|*	Copia identificadores					*
|****************************************************************
|*
|*	idp_t	sidcpy (idp_t sid1, const idp_t sid2);
|*
	.global	_sidcpy
_sidcpy:
	movl	4(sp),r0	|* "sid1"
	movl	8(sp),r1	|* "sid2"

	movl	0(r1),r2
	movl	r2,0(r0)
	movl	4(r1),r2
	movl	r2,4(r0)

	rts
