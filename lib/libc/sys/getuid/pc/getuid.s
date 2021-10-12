|*
|****************************************************************
|*								*
|*			getuid.s				*
|*								*
|*	Obtem a identificação real do usuário			*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: getuid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETUID		= 24
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getuid" *****************************
|*
|*	int	getuid (void);
|*
|*	Retorna = Identificacao real do usuario.
|*
	.global	_getuid
_getuid:
	movl	#GETUID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
