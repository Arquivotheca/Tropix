|*
|****************************************************************
|*								*
|*			execve.s				*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: execve						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	execve (path, argv, envp)
|*	char *path, *argv[], *envp[];
|*
|*	Retorno = Erro: -1
|*
	.global	_execve, _errno
 
EXECE = 59
 
_execve:
	moveq	#EXECE,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0

	movl	d0,_errno
	moveq	#-1,d0
	rts

