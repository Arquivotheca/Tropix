|*
|****************************************************************
|*								*
|*			ffbs.s					*
|*								*
|*	Retorna o índice do primeiro bit não nulo		*
|*								*
|*	Versão	1.0.0, de 04.03.97				*
|*		3.2.3, de 19.12.99				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|*	"ffbs" determina o índice do primeiro bit ligado no argumento
|*	fornecido. Os bits são numerados a partir de 0, do menos
|*	para o mais significativo. Se não houver nenhum, retorna -1
|*
	.text
	.global	_ffbs
_ffbs:
	bsfl	4(sp),r0
	jz	bad
	ret
bad:
	clrl	r0		|* O valor dado é nulo
	decl	r0		|* Retorna -1
	ret

|*
|****************************************************************
|*	Calcula o "log2", usando a instrução "bsfl"		*
|****************************************************************
|*
	.text
	.global	_log2
_log2:
	bsfl	4(sp),r0
	jz	bad

	movl	#1,r2
	movl	r0,r1
	lsll	r1,r2
	cmpl	r2,4(sp)
	jne	bad

	ret
