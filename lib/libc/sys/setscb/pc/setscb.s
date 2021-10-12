|*
|****************************************************************
|*								*
|*			setscb.s				*
|*								*
|*	Altera o bloco de controle do núcleo			*
|*								*
|*	Versão	3.1.0, de 23.10.98				*
|*		3.1.0, de 23.10.98				*
|*								*
|*	Módulo: setscb						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETSCB		= 79
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "setscb" *****************************
|*
|*	int	setscb (SCB *sp);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setscb
_setscb:
	movl	#SETSCB,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
