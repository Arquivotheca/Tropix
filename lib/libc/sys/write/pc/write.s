|*
|****************************************************************
|*								*
|*			write.s					*
|*								*
|*	Escreve em um arquivo					*
|*								*
|*	Versão	1.0.0, de 02.02.87				*
|*		3.0.0, de 17.12.94				*
|*								*
|*	Módulo: write						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

WRITE		= 4
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "write" ******************************
|*
|*	int	write (int fd, void *area, int count);
|*
|*	Retorna = Sucesso: bytes escritos (>= 0); Erro: -1
|*
	.global	_write
_write:
	movl	#WRITE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
