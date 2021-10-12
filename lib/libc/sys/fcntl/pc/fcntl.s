|*
|****************************************************************
|*								*
|*			fcntl.s					*
|*								*
|*	Controle de arquivos					*
|*								*
|*	Versão	3.0.0, de 24.03.95				*
|*		3.0.0, de 24.03.95				*
|*								*
|*	Módulo: fcntl						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

FCNTL		= 65
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "fcntl" ******************************
|*
|*	int	fcntl (int fd, int cmd, int arg);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_fcntl
_fcntl:
	movl	#FCNTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
