|*
|****************************************************************
|*								*
|*			getpid.s				*
|*								*
|*	Obtem a identificação do processo			*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: getpid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETPID		= 20
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getpid" *******************************
|*
|*	int	getpid (void);
|*
|*	Retorna = identificacao do processo
|*
	.global	_getpid
_getpid:
	movl	#GETPID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
