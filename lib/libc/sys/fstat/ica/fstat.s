|*
|****************************************************************
|*								*
|*			fstat.s					*
|*								*
|*	Obtem o estado de um arquivo, dado o descritor		*
|*								*
|*	Versão	1.0.0, de 26.01.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: fstat						*
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
|*	fstat (fd, buf)
|*	int  fd;
|*	STAT *buf;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_fstat, _errno
 
FSTAT = 28

_fstat:
	moveq	#FSTAT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
