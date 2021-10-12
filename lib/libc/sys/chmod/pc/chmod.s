|*
|****************************************************************
|*								*
|*			chmod.s					*
|*								*
|*	Modifica a permissão de acesso a um arquivo		* 
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 12.08.95				*
|*								*
|*	Módulo: chmod						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

CHMOD		= 15
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "chmod" ******************************
|*
|*	int	chmod (char *path, int mode);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_chmod
_chmod:
	movl	#CHMOD,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
