|*
|****************************************************************
|*								*
|*			unlink.s				*
|*								*
|*	Remove uma entrada de um diretorio			*
|*								*
|*	Versão	1.0.0, de 28.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: unlink						*
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
|*	unlink (path)
|*	char *path;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_unlink, _errno
 
UNLINK = 10

_unlink:
	moveq	#UNLINK,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
