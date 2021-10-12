|*
|****************************************************************
|*								*
|*			inopen.s				*
|*								*
|*	Abre um arquivo dado (dev, ino)				*
|*								*
|*	Versão	3.0.0, de 06.01.95				*
|*		3.0.0, de 06.01.95				*
|*								*
|*	Módulo: inopen						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

INOPEN		= 105
SYS_CALL	= { 6 * 8 + 3 }

|*
|*	int	inopen (dev_t dev, ino_t ino);
|*
|*	Retorna = Sucesso: fd; Erro: -1
|*
	.global	_inopen
_inopen:
	movl	#INOPEN,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
