|*
|****************************************************************
|*								*
|*			mknod.s					*
|*								*
|*	Cria um arquivo qualquer				*
|*								*
|*	Versão	1.0.0, de 21.01.86				*
|*		2.3.0, de 08.09.88				*
|*								*
|*	Modulo: mknod						*
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
|*	mknod (path, mode, dev)
|*	char  *path;
|*	int   mode, dev;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mknod, _errno
 
MKNOD = 14

_mknod:
	moveq	#MKNOD,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
