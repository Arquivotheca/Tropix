|*
|****************************************************************
|*								*
|*			times.s					*
|*								*
|*	Obtem os tempos dos processo e seus filhos		*
|*								*
|*	Versão	3.0.0, de 09.02.95				*
|*		3.0.0, de 09.02.95				*
|*								*
|*	Módulo: times						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

TIMES		= 43
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "times" ******************************
|*
|*	#include <sys/types.h>
|*	#include <sys/times.h>
|*
|*	time_t times (TMS *tmsp);
|*
|*	Retorna = Sucesso: Tempo desde o boot; Erro: -1
|*
	.global	_times
_times:
	movl	#TIMES,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
