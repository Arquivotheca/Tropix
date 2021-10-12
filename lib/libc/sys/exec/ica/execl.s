|*
|****************************************************************
|*								*
|*			execl.s					*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	1.0.0, de 08.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: execl						*
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
|*	execl (path, arg0, arg1, ..., argn, (char *)0)
|*	char *path, *arg0, *arg1, ..., *argn;
|*
|*	Retorno = Erro: -1
|*
	.global	_execl
	.global	_environ, _errno
 
EXECE = 59

_execl:
	moveq	#EXECE,d0
	lea	8(sp),a0
	movl	a0,d1
	movl	4(sp),a0
	movl	_environ,a1

	trap	#0

	movl	d0,_errno
	moveq	#-1,d0
	rts

