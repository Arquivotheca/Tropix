|*
|****************************************************************
|*								*
|*			nice.s					*
|*								*
|*	Modifica a prioridade de um processo			*
|*								*
|*	Versão	1.0.0, de 21.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: nice						*
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
|*	nice (incr)
|*	int incr;
|*
|*	Retorna = Sucesso: Novo nice (>= 0); Erro: -1
|*
	.global	_nice, _errno
 
NICE = 34

_nice:
	moveq	#NICE,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
