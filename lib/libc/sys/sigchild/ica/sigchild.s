|*
|****************************************************************
|*								*
|*			sigchild.s				*
|*								*
|*	Prepara o filho para enviar o sinal "SIGCHLD"		*
|*								*
|*	Versão	2.1.0, de 25.04.88				*
|*		2.3.0, de 13.08.88				*
|*								*
|*	Modulo: sigchild					*
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
|*	sigchild (pid)
|*	long	pid;	/* pid do filho */
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_sigchild, _errno

SIGCHILD = 108

_sigchild:
	moveq	#SIGCHILD,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
