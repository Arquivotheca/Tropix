|*
|****************************************************************
|*								*
|*			access.s				*
|*								*
|*	Informa sobre a permissão de acesso a um arquivo	*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: access						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

ACCESS		= 33
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "access" *****************************
|*
|*	int	access (char *path, int mode);
|*
|*	ret =  0 => Acesso é permitido
|*	ret = -1 => Nao é permitido,  ver errno.
|*
	.global	_access
_access:
	movl	#ACCESS,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
