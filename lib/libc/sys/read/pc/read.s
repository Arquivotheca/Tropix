|*
|****************************************************************
|*								*
|*			read.s					*
|*								*
|*	Lê de  um arquivo					*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.0.0, de 19.12.94				*
|*								*
|*	Módulo: read						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

READ		= 3
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "read" *******************************
|*
|*	int	read (int fd, void *area, int count);
|*
|*	Retorna = Sucesso: bytes lidos (>= 0); Erro: -1
|*
	.global	_read
_read:
	movl	#READ,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
