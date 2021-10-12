|*
|****************************************************************
|*								*
|*			mutimes.s				*
|*								*
|*	Obtem o tempo de CPU do processo e filhos		*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: mutimes						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

MUTIMES		= 103
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "mutimes" ****************************
|*
|*	int	mutimes (MUTMS	*tp);
|*
|*	Retorna = Sucesso: tempo de boot; Erro: -1
|*
	.global	_mutimes
_mutimes:
	movl	#MUTIMES,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
