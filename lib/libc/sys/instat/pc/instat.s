|*
|****************************************************************
|*								*
|*			instat.s				*
|*								*
|*	Obtem o estado de um arquivo (dado o dev e ino)		*
|*								*
|*	Versão	3.0.0, de 06.01.95				*
|*		3.0.0, de 06.01.95				*
|*								*
|*	Módulo: instat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

INSTAT		= 122
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "instat" *****************************
|*
|*	int	instat (int dev, int ino, STAT *sp);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_instat
_instat:
	movl	#INSTAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
