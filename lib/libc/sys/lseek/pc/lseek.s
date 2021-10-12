|*
|****************************************************************
|*								*
|*			lseek.s					*
|*								*
|*	Move o ponteiro de posição de um arquivo		*
|*								*
|*	Versão	3.0.0, de 22.12.94				*
|*		3.0.0, de 22.12.94				*
|*								*
|*	Módulo: lseek						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

LSEEK		= 19
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "lseek" *******************************
|*
|*	long	lseek (int fd, off_t offset, int whence)
|*
|*	Retorna = Sucesso: Nova posicao; Erro : -1
|*
	.global	_lseek
_lseek:
	movl	#LSEEK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
