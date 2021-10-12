|*
|****************************************************************
|*								*
|*			mount.s					*
|*								*
|*	Monta um sistema de arquivos				*
|*								*
|*	Versão	1.0.0, de 21.01.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: mount						*
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
|*	mount (blkdev, dir, ronly)
|*	char  *blkdev;
|*	char  *dir;
|*	int   ronly;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mount, _errno
 
MOUNT = 21

_mount:
	moveq	#MOUNT,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
