|*
|****************************************************************
|*								*
|*			semactl.s				*
|*								*
|*	Controle de semáforos do usuário			*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: semactl						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SEMACTL		= 113
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "semactl" ****************************
|*
|*	#include <sys/syscall.h>
|*	#include <unistd.h>
|*
|*	int	semactl (SEMACTLFUNC op, int fd, int arg1, int arg2);
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_semactl
_semactl:
	movl	#SEMACTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
