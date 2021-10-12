|*
|****************************************************************
|*								*
|*			pipe.s					*
|*								*
|*	Cria um canal entre processos				*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: pipe						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

PIPE		= 42
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "pipe" *******************************
|*
|*	int	pipe (int fd[2]);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_pipe
_pipe:
	movl	#PIPE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	movl	4(sp),r2
	movl	r0,0(r2)
	movl	r1,4(r2)

	clrl	r0
	rts
