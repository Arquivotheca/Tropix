|*
|****************************************************************
|*								*
|*			rmdir.s					*
|*								*
|*	Remove um diretório					*
|*								*
|*	Versão	3.0.0, de 19.11.92				*
|*		3.0.0, de 19.11.92				*
|*								*
|*	Modulo: rmdir						*
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
|*	rmdir (char *path)
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_rmdir, _errno
 
RMDIR = 76

_rmdir:
	moveq	#RMDIR,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
