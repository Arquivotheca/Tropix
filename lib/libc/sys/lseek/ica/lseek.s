|*
|****************************************************************
|*								*
|*			lseek.s					*
|*								*
|*	Move o ponteiro de posição de um arquivo		*
|*								*
|*	Versão	1.0.0, de 20.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: lseek						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	long
|*	lseek (fd, offset, whence)
|*	int   fd;
|*	off_t offset;
|*	int   whence;
|*
|*	Retorna = Sucesso: Nova posicao; Erro : -1
|*
	.global	_lseek, _errno
 
LSEEK = 19

_lseek:
	moveq	#LSEEK,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
