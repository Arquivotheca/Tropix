|*
|****************************************************************
|*								*
|*			sidclr.s				*
|*								*
|*	Zera um identificador curto				*
|*								*
|*	Versão	3.0.0, de 14.01.95				*
|*		3.0.0, de 04.07.95				*
|*								*
|*	Módulo: sidclr						*
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
|*	Zera um identificador curto				*
|****************************************************************
|*
|*	idp_t	sidclr (idp_t sid1);
|*
	.global	_sidclr
_sidclr:
	movl	4(sp),r0	|* Zera os 8 bytes de "sid1"

	clrl	r1
	movl	r1,0(r0)
	movl	r1,4(r0)

	rts
