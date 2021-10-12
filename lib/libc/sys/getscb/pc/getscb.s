|*
|****************************************************************
|*								*
|*			getscb.s				*
|*								*
|*	Obtem o bloco de controle do núcleo			*
|*								*
|*	Versão	3.0.0, de 03.02.95				*
|*		3.0.0, de 03.02.95				*
|*								*
|*	Módulo: getscb						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETSCB		= 99
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getscb" *****************************
|*
|*	SCB	*getscb (SCB *sp);
|*
|*	Retorna = Sucesso: &scb; Erro: -1
|*
	.global	_getscb
_getscb:
	movl	#GETSCB,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
