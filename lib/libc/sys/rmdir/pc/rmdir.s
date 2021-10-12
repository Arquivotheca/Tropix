|*
|****************************************************************
|*								*
|*			rmdir.s					*
|*								*
|*	Remove um diretório					*
|*								*
|*	Versão	3.0.0, de 03.02.95				*
|*		3.0.0, de 03.02.95				*
|*								*
|*	Módulo: rmdir						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

RMDIR		= 76
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "rmdir" ******************************
|*
|*	int	rmdir (char *path);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_rmdir
_rmdir:
	movl	#RMDIR,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
