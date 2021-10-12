|*
|****************************************************************
|*								*
|*			execle.s				*
|*								*
|*	Executa um arquivo					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 20.12.94				*
|*								*
|*	Módulo: execle						*
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
|******	Chamada ao sistema "execle" *****************************
|*
|*	int	execle (char *path, char *arg0, ..., char *argn, NOSTR, char *envp[]);
|*
|*	Retorno = Erro: -1
|*
	.global	_execle
_execle:
	link	#0

	leal 	8(fp),r0		|* r0 = envp
1$:
	addl	#4,r0
	cmpl	#0,(r0)
	jne	1$

	addl	#4,r0			|* envp
	pushl	(r0)
	leal	12(fp),r0		|* &arg0
	pushl	r0
	pushl	8(fp)			|* path
	pushl	r0			|* Endereço de retorno "de mentira"

	movl	#EXECE,r0
	callgl	#SYS_CALL,0

	unlk

	jmp	syscall_error
