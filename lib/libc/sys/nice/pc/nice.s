|*
|****************************************************************
|*								*
|*			nice.s					*
|*								*
|*	Altera a prioridade do processo				*
|*								*
|*	Versão	3.0.0, de 11.05.95				*
|*		3.0.0, de 11.05.95				*
|*								*
|*	Módulo: nice						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

NICE		= 34
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "nice" *******************************
|*
|*	int	nice (int incr);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_nice
_nice:
	movl	#NICE,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
