|*
|****************************************************************
|*								*
|*			eventctl.s				*
|*								*
|*	Controle de eventos do usuário				*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: eventctl					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EVENTCTL	 = 109
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "eventctl" ***************************
|*
|*	#include <sys/ipc.h>
|*	#include <sys/syscall.h>
|*
|*	int	eventctl (UEVENTCTLFUNC func, int fd, int arg2);
|*
|*	func = UE_ALLOC: arg2 = no. de eventos
|*	Retorna = Sucesso: 0; Erro: -1
|*
|*	func = UE_GET:   arg2 = id. do evento
|*	Retorna = Sucesso: Descritor do evento; Erro: -1
|*
	.global	_eventctl
_eventctl:
	movl	#EVENTCTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
