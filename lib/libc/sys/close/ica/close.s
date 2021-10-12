|*
|****************************************************************
|*								*
|*			close.s					*
|*								*
|*	Fecha um arquivo, dado o descritor			*
|*								*
|*	Versão	1.0.0, de 09.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: close						*
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
|*	close (fd)
|*	int  fd;
|*
|*	retorno = Sucesso: 0, erro : -1
|*
	.global	_close, _errno
 
CLOSE = 6
 
_close:
	moveq	#CLOSE,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
