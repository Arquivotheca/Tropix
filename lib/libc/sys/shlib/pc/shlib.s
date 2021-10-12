|*
|****************************************************************
|*								*
|*			shlib.s					*
|*								*
|*	Abre um descritor de arquivos				*
|*								*
|*	Versão	3.2.3, de 30.11.99				*
|*		3.2.3, de 29.12.99				*
|*								*
|*	Módulo: shlib						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SHLIB		= 101
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "shlib" ******************************
|*
|*	int	shlib (int op, ...);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_shlib
_shlib:
	movl	#SHLIB,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
