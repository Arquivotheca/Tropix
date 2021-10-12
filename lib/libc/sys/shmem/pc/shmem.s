|*
|****************************************************************
|*								*
|*			shmem.s					*
|*								*
|*	Obtém regiões de memória compartilhada para o usuário	*
|*								*
|*	Versão	3.0.0, de 21.08.95				*
|*		3.1.5, de 05.12.98				*
|*								*
|*	Módulo: shmem						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SHMEM		= 117
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "shmem" ******************************
|*
|*	#include <sys/syscall.h>
|*	#include <sys/unistd.h>
|*
|*	int	shmem (int fd, int size);
|*
|*	Retorna = Sucesso: Endereço virtual da região; Erro: -1
|*
	.global	_shmem
_shmem:
	movl	#SHMEM,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
