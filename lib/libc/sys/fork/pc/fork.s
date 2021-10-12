|*
|****************************************************************
|*								*
|*			fork.s					*
|*								*
|*	Cria um processo					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 20.12.94				*
|*								*
|*	Módulo: fork						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

FORK		= 2
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "fork" *******************************
|*
|*	int	fork (void);
|*
|*	Retorna no Pai   = Sucesso: Pid do filho; Erro: -1
|*	Retorna no Filho = Sucesso: 0
|*
	.global	_fork
_fork:
	movl	#FORK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
