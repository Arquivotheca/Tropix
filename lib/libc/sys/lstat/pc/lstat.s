|*
|****************************************************************
|*								*
|*			lstat.s					*
|*								*
|*	Obtem o estado de um arquivo (NÃO acompanha os elos)	*
|*								*
|*	Versão	4.2.0, de 15.04.02				*
|*		4.2.0, de 15.04.02				*
|*								*
|*	Módulo: lstat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

LSTAT		= 84
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "lstat" ******************************
|*
|*	int	lstat (const char *path, STAT *buf);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_lstat
_lstat:
	movl	#LSTAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
