|*
|****************************************************************
|*								*
|*			stime.s					*
|*								*
|*	Acerta o relogio do sistema				*
|*								*
|*	Versão	1.0.0, de 26.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: stime						*
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
|*	stime (tp)
|*	time_t *tp;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_stime, _errno
 
STIME = 25

_stime:
	moveq	#STIME,d0
	movl	4(sp),a0
	movl	(a0),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
