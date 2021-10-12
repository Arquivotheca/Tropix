|*
|****************************************************************
|*								*
|*			stat.s					*
|*								*
|*	Obtem o estado de um arquivo (acompanha os elos)	*
|*								*
|*	Versão	3.0.0, de 05.01.95				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: stat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

STAT		= 18
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "stat" *******************************
|*
|*	int	stat (char *path, STAT *buf);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_stat
_stat:
	movl	#STAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
