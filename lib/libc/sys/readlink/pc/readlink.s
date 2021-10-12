|*
|****************************************************************
|*								*
|*			readlink.s				*
|*								*
|*	Lê o valor de um elo simbólico				*
|*								*
|*	Versão	4.2.0, de 05.04.02				*
|*		4.2.0, de 16.04.02				*
|*								*
|*	Módulo: readlink					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

READLINK	= 83
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "readlink" ***************************
|*
|*	int	readlink (const char *path, char *area, int count);
|*
|*	Retorna = Sucesso: bytes lidos (>= 0); Erro: -1
|*
	.global	_readlink
_readlink:
	movl	#READLINK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
