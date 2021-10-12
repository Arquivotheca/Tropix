|*
|****************************************************************
|*								*
|*			getgid.s				*
|*								*
|*	Obtem a identificacao real do grupo			*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: getgid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETGID		= 47
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getgid" *****************************
|*
|*	int	getgid (void);
|*
|*	Retorna = Identificacao real do grupo
|*
	.global	_getgid
_getgid:
	movl	#GETGID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
