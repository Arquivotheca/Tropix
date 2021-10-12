|*
|****************************************************************
|*								*
|*			kill.s					*
|*								*
|*	Envia um sinal para processos				*
|*								*
|*	Versão	3.0.0, de 04.02.95				*
|*		3.0.0, de 04.02.95				*
|*								*
|*	Módulo: kill						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

KILL		= 37
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "kill" ******************************
|*
|*	int	kill (long pid, int signo);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_kill
_kill:
	movl	#KILL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
