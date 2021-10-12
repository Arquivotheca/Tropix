|*
|****************************************************************
|*								*
|*			setgid.s				*
|*								*
|*	Estabelece o GID do processo				*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: setgid						*
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
|*	setgid (gid)
|*	int gid;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setgid, _errno
 
SETGID = 46

_setgid:
	moveq	#SETGID,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
