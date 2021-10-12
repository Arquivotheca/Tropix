|*
|****************************************************************
|*								*
|*			close.s					*
|*								*
|*	Fecha um arquivo, dado o descritor			*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.0.0, de 12.08.95				*
|*								*
|*	Módulo: close						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

CLOSE		= 6
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "close" ******************************
|*
|*	int	close (int fd);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_close
_close:
	movl	#CLOSE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
