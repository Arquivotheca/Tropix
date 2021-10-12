|*
|****************************************************************
|*								*
|*			stime.s					*
|*								*
|*	Acerta o relogio do sistema				*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: stime						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

STIME		= 25
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "stime" ******************************
|*
|*	int	stime (time_t *tp);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_stime
_stime:
	movl	#STIME,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
