|*
|****************************************************************
|*								*
|*			getdirpar.s				*
|*								*
|*	Lê de um diretório com um formato independente		*
|*								*
|*	Versão	4.0.0, de 02.08.01				*
|*		4.0.0, de 02.08.01				*
|*								*
|*	Módulo: getdirpar					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

GETDIRPAR	= 127
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "getdirpar" **************************
|*
|*	int	getdirpar (dev_t dev, ino_t ino, DIRENT *dep);
|*
	.global	_getdirpar
_getdirpar:
	movl	#GETDIRPAR,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
