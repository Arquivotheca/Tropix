|*
|****************************************************************
|*								*
|*			inopen.s				*
|*								*
|*	Abre um arquivo dado (dev, ino)				*
|*								*
|*	Versão	1.0.0, de 03.02.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: inopen						*
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
|*	iopen (dev, ino)
|*	dev_t dev;
|*	ino_t ino;
|*
|*	Retorna = Sucesso: fd; Erro: -1
|*
	.global	_inopen, _errno
 
INOPEN = 105

_inopen:
	moveq	#INOPEN,d0		|* INOPEN
	movl	4(sp),a0		|* dev
	movl	8(sp),d1		|* ino

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
