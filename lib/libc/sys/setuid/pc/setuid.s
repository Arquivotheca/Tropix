|*
|****************************************************************
|*								*
|*			setuid.s				*
|*								*
|*	Atribui o UID do processo				*
|*								*
|*	Versão	3.0.0, de 01.01.95				*
|*		3.0.0, de 01.01.95				*
|*								*
|*	Módulo: setuid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETUID		= 23
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "setuid" *****************************
|*
|*	int	setuid (int uid);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setuid
_setuid:
	movl	#SETUID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
