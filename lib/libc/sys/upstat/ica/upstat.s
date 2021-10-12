|*
|****************************************************************
|*								*
|*			upstat.s				*
|*								*
|*   Obtem o estado de um arquivo (pai de um sistema montado)	*
|*								*
|*	Versão	3.0.4, de 11.12.93				*
|*		3.0.4, de 11.12.93				*
|*								*
|*	Modulo: upstat						*
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
|*	upstat (dev_t dev, STAT *sp)
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_upstat, _errno
 
UPSTAT = 123

_upstat:
	moveq	#UPSTAT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
