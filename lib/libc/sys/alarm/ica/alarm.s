|*
|****************************************************************
|*								*
|*			alarm.s					*
|*								*
|*	Envia o sinal "SIGALRM" apos "n" segundos		*
|*								*
|*	Versão	1.0.0, de 13.05.86				*
|*		2.3.0, de 02.09.88				*
|*								*
|*	Módulo: alarm.s						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	unsigned int
|*	alarm (seconds)
|*	unsigned int seconds;
|*
|*	Retorna o tempo que estava no "alarm clock"
|*
	.global	_alarm
 
ALARM = 27

_alarm:
	moveq	#ALARM,d0
	movl	4(sp),a0

	trap	#0
	rts
