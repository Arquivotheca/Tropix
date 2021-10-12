|*
|****************************************************************
|*								*
|*			itnetsnd.s				*
|*								*
|*	Executa a função "t_snd" da INTERNET			*
|*								*
|*	Versão	3.0.0, de 11.05.95				*
|*		3.0.0, de 11.05.95				*
|*								*
|*	Módulo: itnetsnd					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

ITNETSND	= 126
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "itnetsnd" ***************************
|*
|*	int	itnetsnd (int fd, void *area, int count, int *flags)
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_itnetsnd
_itnetsnd:
	movl	#ITNETSND,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
