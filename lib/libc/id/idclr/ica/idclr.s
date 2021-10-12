|*
|****************************************************************
|*								*
|*			idclr.s					*
|*								*
|*	Zera um identificador					*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Rotinas:						*
|*		idclr						*
|*								*
|*	Módulo: idclr						*
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
|*	Zera um Identificador					*
|****************************************************************
|*
|*	idp_t
|*	idclr (id)
|*	idp_t	id;
|*
	.global	_idclr

_idclr:
	movl	4(sp),a0	|* id
	movl	a0,d0

	clrl	(a0)+		|* Zera os 16 bytes
	clrl	(a0)+
	clrl	(a0)+
	clrl	(a0)

	rts
