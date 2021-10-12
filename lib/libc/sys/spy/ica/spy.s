|*
|****************************************************************
|*								*
|*			spy.s					*
|*								*
|*	Chama a rotina de instrumentação do núcleo		*
|*								*
|*	Versão	1.0.0, de 03.02.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: spy						*
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
|*	spy ()
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_spy, _errno

SPY = 100

_spy:
	moveq	#SPY,d0	
	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
