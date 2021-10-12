|*
|****************************************************************
|*								*
|*			attention.s				*
|*								*
|*	Multiplexação de entrada/saída síncrona			*
|*								*
|*	Versão	2.3.10, de 30.04.91				*
|*		2.3.11, de 18.06.91				*
|*								*
|*	Modulo: attention					*
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
|*	attention (int nfd, int fd_vector[], int index, int timeout)
|* 
|*	Retorno: Sucesso: Index (>= 0); Erro ou timeout: -1
|* 
	.global	_attention, _errno

ATTENTION  =  119

_attention:
	movl	d2,-(sp)

	moveq	#ATTENTION,d0
	movl	8(sp),a0
	movl	12(sp),d1
	movl	16(sp),a1
	movl	20(sp),d2

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	movl	(sp)+,d2
	rts
