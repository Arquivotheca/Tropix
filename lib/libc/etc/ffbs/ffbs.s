|*
|****************************************************************
|*								*
|*			ffbs.s					*
|*								*
|*	Retorna o índice do primeiro bit não nulo		*
|*								*
|*	Versão	3.2.3, de 14.12.99				*
|*		3.2.3, de 26.12.99				*
|*								*
|*	Módulo: ffbs						*
|*		lib/etc						*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|*	int ffbs (unsigned long value);
|*
|*	"ffbs" determina o índice do primeiro bit ligado no argumento
|*	fornecido. Os bits são numerados a partir de 0, do menos
|*	para o mais significativo. Se não houver bit algum, retorna -1
|*
	.text
	.global	_ffbs
_ffbs:
	bsfl	4(sp),r0
	jz	1$
	ret
1$:
	clrl	r0		|* O valor dado é nulo
	decl	r0		|* Retorna -1
	ret
