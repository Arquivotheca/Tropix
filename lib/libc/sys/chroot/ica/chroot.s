|*
|****************************************************************
|*								*
|*			chroot.s				*
|*								*
|*	Muda o diretório raiz					*
|*								*
|*	Versão	1.0.0, de 09.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: chroot						*
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
|*	chroot (path)
|*	char *path;
|*
|*	Retorno = Sucesso: 0, Erro: -1
|*
	.global	_chroot, _errno
 
CHROOT = 61
 
_chroot:
	moveq	#CHROOT,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
