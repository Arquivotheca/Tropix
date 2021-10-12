|*
|****************************************************************
|*								*
|*			sidclr.s				*
|*								*
|*	Zera um identificador					*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 17.04.89				*
|*								*
|*	Rotinas:						*
|*		sidclr						*
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
|*	Zera um Identificador					*
|****************************************************************
|*
|*	idp_t
|*	sidclr (sid)
|*	idp_t	sid;
|*
	.global	_sidclr

_sidclr:
	movl	4(sp),a0	|* sid
	movl	a0,d0

	clrl	(a0)+		|* Zera os 8 bytes
	clrl	(a0)

	rts
