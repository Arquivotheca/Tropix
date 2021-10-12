|*
|****************************************************************
|*								*
|*			times.s					*
|*								*
|*	Obtem os tempos dos processo e seus filhos		*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: times						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	#include <sys/types.h>
|*	#include <sys/times.h>
|*
|*	time_t
|*	times (buffer)
|*	TMS *buffer;
|*
|*	Retorna = Sucesso: Tempo desde o boot; Erro: -1
|*
	.global	_times, _errno
 
TIMES = 43

_times:
	moveq	#TIMES,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
