|*
|****************************************************************
|*								*
|*			eventwait.s				*
|*								*
|*	Obtém o número do último evento de usuário ocorrido	*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: eventwait					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EVENTWAIT	= 110
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "eventwait" ***************************
|*
|*	#include <sys/ipc.h>
|*	#include <sys/syscall.h>
|*
|*	int	eventwait (int eventdescr, int eventno);
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventwait
_eventwait:
	movl	#EVENTWAIT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
