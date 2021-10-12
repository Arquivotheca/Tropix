|*
|****************************************************************
|*								*
|*			umount.s				*
|*								*
|*	Desmonta um sistema de arquivos				*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: umount						*
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
|*	umount (blkdev)
|*	char *blkdev;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_umount, _errno
 
UMOUNT = 22

_umount:
	moveq	#UMOUNT,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
