|*
|****************************************************************
|*								*
|*			ustat.s					*
|*								*
|*	Obtem o estado de um sistema de arquivos		*
|*								*
|*	Versão	1.0.0, de 29.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: ustat						*
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
|*	ustat (dev, buf)
|*	dev_t	dev;
|*	USTAT	*buf;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_ustat, _errno

USTAT = 72

_ustat:
	moveq	#USTAT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
