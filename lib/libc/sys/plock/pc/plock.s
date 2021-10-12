|*
|****************************************************************
|*								*
|*			plock.s					*
|*								*
|*	Mantem/libera um processo na memória interna		*
|*								*
|*	Versão	3.0.0, de 18.08.95				*
|*		3.0.0, de 18.08.95				*
|*								*
|*	Módulo: plock						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

PLOCK		= 97
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "plock" ******************************
|*
|*	int	plock (int flag);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_plock
_plock:
	movl	#PLOCK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
