|*
|****************************************************************
|*								*
|*			eventtest.s				*
|*								*
|*	Obtém o número do último evento de usuário ocorrido	*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: eventtest					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EVENTTEST	= 112
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "eventtest" ***************************
|*
|*	#include <sys/ipc.h>
|*	#include <sys/syscall.h>
|*
|*	int	eventtest (int eventdescr);
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventtest
_eventtest:
	movl	#EVENTTEST,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
