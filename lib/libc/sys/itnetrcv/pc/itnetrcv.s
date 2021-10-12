|*
|****************************************************************
|*								*
|*			itnetrcv.s				*
|*								*
|*	Executa a função "t_rcv" da INTERNET			*
|*								*
|*	Versão	3.0.0, de 11.05.95				*
|*		3.0.0, de 11.05.95				*
|*								*
|*	Módulo: itnetrcv					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

ITNETRCV	= 125
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "itnetrcv" ***************************
|*
|*	int	itnetrcv (int fd, void *area, int count, int *flags)
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_itnetrcv
_itnetrcv:
	movl	#ITNETRCV,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
