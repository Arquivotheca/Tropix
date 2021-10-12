|*
|****************************************************************
|*								*
|*			eventdone.s				*
|*								*
|*	Sinaliza a ocorrência de um evento do usuário		*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: eventdone					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EVENTDONE	= 111
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "eventdone" **************************
|*
|*	#include <sys/ipc.h>
|*	#include <sys/syscall.h>
|*
|*	int	eventdone (int eventdescr);
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventdone
_eventdone:
	movl	#EVENTDONE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
