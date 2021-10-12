|*
|****************************************************************
|*								*
|*			setppid.s				*
|*								*
|*	Altera o pai do processo				*
|*								*
|*	Versão	2.3.12, de 13.10.91				*
|*		2.3.12, de 13.10.91				*
|*								*
|*	Modulo: setppid						*
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
|*	setppid (long ppid);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setppid, _errno
 
SETPPID = 121

_setppid:
	moveq	#SETPPID,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
