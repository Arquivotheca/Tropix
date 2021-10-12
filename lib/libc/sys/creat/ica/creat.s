|*
|****************************************************************
|*								*
|*			creat.s					*
|*								*
|*	Cria um arquivo novo ou escreve em um antigo		*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: creat						*
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
|*	creat (path, mode)
|*	char *path;
|*	int  mode;
|*
|*	Retorno = Sucesso: um fd >= 0; Erro: -1
|*
	.global	_creat, _errno
 
CREAT = 8 

_creat:
	moveq	#CREAT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
