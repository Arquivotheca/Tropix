|*
|****************************************************************
|*								*
|*			time.s					*
|*								*
|*	Obtem o tempo do sistema				*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: time						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	time_t
|*	time (tloc)
|*	time_t  *tloc
|*
|*	Retorna = Sucesso: Tempo (> 0); Erro: -1
|*
	.global	_time
 
TIME = 13

_time:
	moveq	#TIME,d0
	trap	#0

	tstl	4(sp)
	jeq	1$

	movl	4(sp),a0
	movl	d0,(a0)
1$:
	rts
