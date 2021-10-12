|*
|****************************************************************
|*								*
|*			ustat.s					*
|*								*
|*	Obtem o estado de um sistema de arquivos		*
|*								*
|*	Versão	3.0.0, de 14.01.95				*
|*		3.0.0, de 14.01.95				*
|*								*
|*	Módulo: ustat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

USTAT		= 72
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "ustat" *******************************
|*
|*	int	ustat (dev_t dev, USTAT *up);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_ustat
_ustat:
	movl	#USTAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
