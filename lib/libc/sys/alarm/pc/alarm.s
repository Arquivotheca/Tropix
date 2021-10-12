|*
|****************************************************************
|*								*
|*			alarm.s					*
|*								*
|*	Envia o sinal "SIGALRM" apos "n" segundos		*
|*								*
|*	Versão	3.0.0, de 04.02.95				*
|*		3.0.0, de 11.08.95				*
|*								*
|*	Módulo: alarm						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

ALARM		= 27
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "alarm" ******************************
|*
|*	unsigned int	alarm (unsigned int seconds);
|*
|*	Retorna o tempo que estava no "alarm clock"
|*
	.global	_alarm
_alarm:
	movl	#ALARM,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
