|*
|****************************************************************
|*								*
|*			instat.s				*
|*								*
|*	Obtem o estado de um arquivo (dado o dev e ino)		*
|*								*
|*	Versão	3.0.4, de 10.12.93				*
|*		3.0.4, de 10.12.93				*
|*								*
|*	Modulo: instat						*
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
|*	instat (dev_t dev, ino_t ino, STAT *sp)
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_instat, _errno
 
INSTAT = 122

_instat:
	moveq	#INSTAT,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
