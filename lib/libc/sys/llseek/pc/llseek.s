|*
|****************************************************************
|*								*
|*			llseek.s				*
|*								*
|*	Move o ponteiro de posição de um arquivo		*
|*								*
|*	Versão	4.4.0, de 28.10.02				*
|*		4.4.0, de 28.10.02				*
|*								*
|*	Módulo: llseek						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

LLSEEK		= 86
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "llseek" ******************************
|*
|*	int	llseek (int fd, loff_t offset, loff_t *result, int whence)
|*
|*	Retorna = Sucesso: 0; Erro : -1
|*
	.global	_llseek
_llseek:
	movl	#LLSEEK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
