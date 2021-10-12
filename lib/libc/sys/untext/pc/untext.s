|*
|****************************************************************
|*								*
|*			untext.s				*
|*								*
|*	Desaloca os recursos de um "text" ativo			*
|*								*
|*	Versão	3.0.0, de 23.08.95				*
|*		3.0.0, de 23.08.95				*
|*								*
|*	Módulo: untext						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

UNTEXT		= 107
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "untext" *****************************
|*
|*	int	untext (char *path);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_untext
_untext:
	movl	#UNTEXT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
