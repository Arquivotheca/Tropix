|*
|****************************************************************
|*								*
|*			getpgrp.s				*
|*								*
|*	Obtem a identificação do grupo de processos		*
|*								*
|*	Versão	3.0.0, de 14.08.95				*
|*		3.0.0, de 14.08.95				*
|*								*
|*	Módulo: getpgrp						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETPGRP		= 66
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getpgrp" *****************************
|*
|*	long	getpgrp (void);
|*
|*	Retorna = identificacao do grupo de processos
|*
	.global	_getpgrp
_getpgrp:
	movl	#GETPGRP,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
