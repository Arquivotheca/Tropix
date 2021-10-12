|*
|****************************************************************
|*								*
|*			upstat.s				*
|*								*
|*   Obtem o estado de um arquivo (pai de um sistema montado)	*
|*								*
|*	Versão	3.0.0, de 06.01.95				*
|*		3.0.0, de 06.01.95				*
|*								*
|*	Módulo: upstat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

UPSTAT		= 123
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "upstat" *****************************
|*
|*	int	upstat (dev_t dev, STAT *sp);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_upstat
_upstat:
	movl	#UPSTAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
