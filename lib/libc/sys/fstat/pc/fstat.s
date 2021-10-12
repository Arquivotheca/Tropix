|*
|****************************************************************
|*								*
|*			fstat.s					*
|*								*
|*	Obtem o estado de um arquivo, dado o descritor		*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 15.01.95				*
|*								*
|*	Módulo: fstat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

FSTAT		= 28
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "fstat" ******************************
|*
|*	int	fstat (int fd, STAT *sp);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_fstat
_fstat:
	movl	#FSTAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
