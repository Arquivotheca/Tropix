|*
|****************************************************************
|*								*
|*			unlink.s				*
|*								*
|*	Remove uma entrada de um diretorio			*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: unlink						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

UNLINK		= 10
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "unlink" *****************************
|*
|*	int	unlink (char *path);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_unlink
_unlink:
	movl	#UNLINK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
