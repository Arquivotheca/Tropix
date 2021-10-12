|*
|****************************************************************
|*								*
|*			setrgid.s				*
|*								*
|*	Estabelece o RGID do processo				*
|*								*
|*	Versão	3.0.0, de 02.11.92				*
|*		3.0.0, de 02.11.92				*
|*								*
|*	Modulo: setrgid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	int
|*	setrgid (rgid)
|*	int rgid;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setrgid, _errno
 
SETREGID = 74

_setrgid:
	moveq	#SETREGID,d0
	movl	4(sp),a0	|* rgid
	moveq	#-1,d1		|* egid

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
