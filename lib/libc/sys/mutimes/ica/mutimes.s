|*
|****************************************************************
|*								*
|*			mutimes.s				*
|*								*
|*	Obtem o tempo de CPU do processo e filhos		*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: mutimes						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	int
|*	mutimes (tp)
|*	MUTMS	*tp;
|*
|*	Retorna = Sucesso: tempo de boot; Erro: -1
|*
	.global	_mutimes, _errno

MUTIMES	= 103

_mutimes:
	movl	4(sp),a0		|* a0 = Endereco da Estrutura.
	moveq	#MUTIMES,d0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
