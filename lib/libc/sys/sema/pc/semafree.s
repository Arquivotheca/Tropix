|*
|****************************************************************
|*								*
|*			semafree.s				*
|*								*
|*	Controle de semáforos do usuário			*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: semafree					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SEMAFREE	= 115
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "semafree" ***************************
|*
|*	#include <sys/syscall.h>
|*	#include <unistd.h>
|*
|*	int	semafree (int op, int fd, int arg1, int arg2);
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_semafree
_semafree:
	movl	#SEMAFREE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
