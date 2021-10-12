|*
|****************************************************************
|*								*
|*			fp.s					*
|*								*
|*	Obtém o valor do registrador "fp"			*
|*								*
|*	Versão	1.0.0, de 04.03.97				*
|*								*
|*	Módulo: xc/lib/aux					*
|*		X Windows - Bibliotecas				*
|*		Baseado no software homônimo do XFree86		*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright (c) 1997 TROPIX Technologies Ltd.	*
|* 								*
|****************************************************************
|*

	.text
	.global	_get_fp

_get_fp:
	movl	fp,r0
	rts
