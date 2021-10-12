|*
|****************************************************************
|*								*
|*			setegid.s				*
|*								*
|*	Estabelece o EGID do processo				*
|*								*
|*	Versão	3.0.0, de 02.11.92				*
|*		3.0.0, de 02.11.92				*
|*								*
|*	Modulo: setegid						*
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
|*	setegid (egid)
|*	int egid;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setegid, _errno
 
SETREGID = 74

_setegid:
	moveq	#SETREGID,d0
	movw	#-1,a0		|* rgid
	movl	4(sp),d1	|* egid

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
