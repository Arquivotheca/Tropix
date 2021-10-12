|*
|****************************************************************
|*								*
|*			getegid.s				*
|*								*
|*	Obtem a identificação efetiva do grupo			*
|*								*
|*	Versão	3.0.0, de 14.08.95				*
|*		3.0.0, de 14.08.95				*
|*								*
|*	Módulo: getegid						*
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
|******	Chamada ao sistema "getegid" *******************************
|*
|*	int	getegid (void);
|*
|*	Retorna = Identificação efetiva do grupo
|*
	.global	_getegid
_getegid:
	movl	#GETGID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	movl	r1,r0
	rts
