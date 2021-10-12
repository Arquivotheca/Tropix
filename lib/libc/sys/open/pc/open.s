|*
|****************************************************************
|*								*
|*			open.s					*
|*								*
|*	Abre um descritor de arquivos				*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		3.0.0, de 13.12.94				*
|*								*
|*	Módulo: open						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

OPEN		= 5
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "open" *******************************
|*
|*	int	open (char *path, int oflag, int mode);
|*
|*	Retorna = Sucesso: fd (>= 0); Erro: -1
|*
	.global	_open
_open:
	movl	#OPEN,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
