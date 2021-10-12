|*
|****************************************************************
|*								*
|*			thread.s				*
|*								*
|*	Cria um processo					*
|*								*
|*	Versão	3.0.0, de 11.05.95				*
|*		3.0.0, de 11.05.95				*
|*								*
|*	Módulo: thread						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

THREAD		= 118
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "thread" *****************************
|*
|*	int	thread (void);
|*
|*	Retorna no Pai   = Sucesso: Pid do filho; Erro: -1
|*	Retorna no Filho = Sucesso: 0
|*
	.global	_thread
_thread:
	movl	#THREAD,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
