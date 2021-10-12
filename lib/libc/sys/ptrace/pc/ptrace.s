|*
|****************************************************************
|*								*
|*			ptrace.s				*
|*								*
|*	Executa os comandos de TRACE				*
|*								*
|*	Versão	3.0.0, de 18.08.95				*
|*		3.0.0, de 18.08.95				*
|*								*
|*	Módulo: ptrace						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

PTRACE		= 26
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "ptrace" *****************************
|*
|*	int	ptrace (int req, int pid, int addr, int data);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_ptrace
_ptrace:
	movl	#PTRACE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
