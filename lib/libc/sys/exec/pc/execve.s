|*
|****************************************************************
|*								*
|*			execve.s				*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 20.12.94				*
|*								*
|*	Módulo: execve						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EXECE		= 59
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "execve" *****************************
|*
|*	int	execve (char *path, char *argv[], char *envp[]);
|*
|*	Retorno = Erro: -1
|*
	.global	_execve
_execve:
	movl	#EXECE,r0
	callgl	#SYS_CALL,0

	jmp	syscall_error
