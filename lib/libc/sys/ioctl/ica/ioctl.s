|*
|****************************************************************
|*								*
|*			ioctl.s					*
|*								*
|*	Controla dispositivos					*
|*								*
|*	Versão	1.0.0, de 16.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: ioctl						*
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
|*	ioctl (fd, cmd, arg)
|*	int fd, cmd, arg;
|*
|*	Retorno = Sucesso: >= 0; Erro: -1
|*
	.global	_ioctl, _errno
 
IOCTL = 54
 
_ioctl:
	moveq	#IOCTL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
