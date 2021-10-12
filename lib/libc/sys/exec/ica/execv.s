|*
|****************************************************************
|*								*
|*			execv.s					*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: execv						*
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
|*	execv (path, argv)
|*	char *path, *argv[];
|*
|*	Retorno = Erro: -1
|*
	.global	_execv
	.global	_environ, _errno
 
EXECE = 59

_execv:
	moveq	#EXECE,d0
	movl 	4(sp),a0
	movl	8(sp),d1
	movl	_environ,a1

	trap	#0

	movl	d0,_errno
	moveq	#-1,d0
	rts

