|*
|****************************************************************
|*								*
|*			setppid.s				*
|*								*
|*	Altera o pai de um processo				*
|*								*
|*	Versão	3.0.0, de 11.05.95				*
|*		3.0.0, de 11.05.95				*
|*								*
|*	Módulo: setppid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETPPID		= 121
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "setppid" ****************************
|*
|*	int	setppid (void);
|*
|*	Retorna = >= 0: sucesso; -1: erro
|*
	.global	_setppid
_setppid:
	movl	#SETPPID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
