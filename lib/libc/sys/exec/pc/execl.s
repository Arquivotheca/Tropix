|*
|****************************************************************
|*								*
|*			execl.s					*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 20.12.94				*
|*								*
|*	Módulo: execl						*
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
|******	Chamada ao sistema "execl" ******************************
|*
|*	int	execl (char *path, char *arg0, ..., char *argn, NOSTR);
|*
|*	Retorno = Erro: -1
|*
	.global	_execl
_execl:
	link	#0

	pushl	_environ		|* environ
	leal	12(fp),r0		|* &arg0
	pushl	r0
	pushl	8(fp)			|* path
	pushl	r0			|* Endereço de retorno "de mentira"

	movl	#EXECE,r0
	callgl	#SYS_CALL,0

	unlk

	jmp	syscall_error
