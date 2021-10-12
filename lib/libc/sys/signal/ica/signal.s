|*
|****************************************************************
|*								*
|*			signal.s				*
|*								*
|*	Gerencia de sinais					*
|*								*
|*	Versão	1.0.0, de 23.01.87				*
|*		2.3.0, de 08.04.88				*
|*								*
|*	Modulo: signal						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	#include <signal.h>
|*
|*	int
|*	(*signal (sig, func)) ()
|*	int sig;
|*	int (*func) (signo, pc, fa);
|*
|*	Retorna = Sucesso: func antigo; Erro: -1
|*
	.global	_signal, _errno
 
SIGNAL = 48

_signal:
	moveq	#SIGNAL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	#sigludium,a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts


|*
|*	Interludio entre a interceptação do sinal no
|*	núcleo e a chamada à função interceptadora.
|*
sigludium:
	moveml	#<a0-a6,d0-d7>,-(sp)	|* Guarda os registradores

	movl	72(sp),a0		|* a0: funcao interceptadora

	movl	68(sp),-(sp)		|* 3. arg: Fault address

	movl	68(sp),-(sp)		|* 2. arg: Fault pc

	movl	68(sp),-(sp)		|* 1. arg: No. do sinal

	jsr	(a0)			|* Chama a funcao

	addw	#12,sp			|* Retira (signo, pc, fa)

	moveml	(sp)+,#<a0-a6,d0-d7>	|* Restaura os registradores

	addw	#16,sp			|* Retira: signo, PC, FA, func 

	rtr				|* Retorna ao processo
