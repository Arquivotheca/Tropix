|*
|****************************************************************
|*								*
|*			ffs.s					*
|*								*
|*	Retorna o índice do primeiro bit não nulo		*
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
	.global	_ffs
|*
|*	ffs determina o índice do primeiro bit ligado no argumento
|*	fornecido. Os bits são numerados a partir de 1, do menos
|*	para o mais significativo.
|*
_ffs:
	bsfl	4(sp),r0
	jz	1$
	incl	r0		|* a numeração começa de 1
	ret

1$:	clrl	r0		|* o valor dado é nulo
	ret
