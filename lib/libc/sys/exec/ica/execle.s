|*
|****************************************************************
|*								*
|*			execle.s				*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: execle						*
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
|*	execle (path, arg0, arg1, ..., argn, (char *)0, envp)
|*	char *path, *arg0, *arg1, ...,*argn, *envp[];
|*
|*	Retorno = Erro: -1
|*
	.global	_execle, _errno
 
EXECE = 59

_execle:
	moveq	#EXECE,d0

	lea 	8(sp),a0	|* a1 = envp
1$:
	tstl	(a0)+
	jne	1$

	movl 	(a0),a1
 
	lea 	8(sp),a0	|* d1 = argp
	movl	a0,d1
 
	movl	4(sp),a0	|* a0 = path

	trap	#0

	movl	d0,_errno
	moveq	#-1,d0
	rts
