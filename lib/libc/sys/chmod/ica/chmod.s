|*
|****************************************************************
|*								*
|*			chmod.s					*
|*								*
|*	Modifica a permissao de acesso de um arquivo		* 
|*								*
|*	Versão	1.0.0, de 08.01.86				*
|*		2.3.0, de 05.07.88				*
|*								*
|*	Modulo: chmod						*
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
|*	chmod (path, mode)
|*	char *path;
|*	int  mode;
|*
|*	Retorno = Sucesso: 0, Erro: -1
|*
	.global	_chmod, _errno
 
CHMOD = 15
 
_chmod:
	moveq	#CHMOD,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
