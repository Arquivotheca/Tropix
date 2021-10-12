|*
|****************************************************************
|*								*
|*			signal.s				*
|*								*
|*	Gerencia de sinais					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 10.01.95				*
|*								*
|*	Módulo: signal						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SIGNAL		= 48
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "signal" *******************************
|*
|*	#include <signal.h>
|*
|*	void	(*signal (int sig, void (*func) (int signo, int pc, int fa)));
|*
|*	Retorna = Sucesso: func antigo; Erro: -1
|*
	.global	_signal
_signal:
	link	#0

	pushl	#sigludium		|* sigludium
	pushl	12(fp)			|* func
	pushl	8(fp)			|* sig
	pushl	r0			|* Endereço de retorno "de mentira"

	movl	#SIGNAL,r0
	callgl	#SYS_CALL,0

	unlk

	tstl	r2
	jnz	syscall_error

	rts

|*
|*	Interlúdio entre a interceptação do sinal no
|*	núcleo e a chamada à função interceptadora.
|*
sigludium:
	pushal				|* Guarda os registros

	link	#0			|* Prepara uma referencia sólida

	pushl	11*4(fp)		|* 3. arg: Fault address
	pushl	10*4(fp)		|* 2. arg: Fault pc
	pushl	9*4(fp)			|* 1. arg: No. do sinal

	call	12*4(fp)		|* Chama a função
	addl	#3*4,sp

	unlk

	popal				|* Restaura os registros

	addl	#4*4,sp			|* Retira: signo, PC, FA, func 

	popfl				|* restaura o SR

	rts				|* Retorna ao processo
