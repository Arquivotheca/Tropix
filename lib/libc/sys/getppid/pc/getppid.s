|*
|****************************************************************
|*								*
|*			getppid.s				*
|*								*
|*	Obtem a identificação do pai do processo		*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: getppid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETPPID		= 67
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getppid" ****************************
|*
|*	int	getppid (void);
|*
|*	Retorna = identificacao do pai do processo
|*
	.global	_getppid
_getppid:
	movl	#GETPPID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
