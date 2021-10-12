|*
|****************************************************************
|*								*
|*			lockf.s					*
|*								*
|*	Gerencia LOCKs em regiões de arquivos			*
|*								*
|*	Versão	3.0.0, de 16.08.95				*
|*		3.0.0, de 16.08.95				*
|*								*
|*	Módulo: lockf						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

LOCKF	 	= 56
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "lockf" ******************************
|*
|*	int	lockf (int fd, int mode, int size);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_lockf
_lockf:
	movl	#LOCKF,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
