|*
|****************************************************************
|*								*
|*			pause.s					*
|*								*
|*	Suspende o processo até a ocorrencia de um sinal	*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: pause						*
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
|*	pause ()
|*
|*	Retorna = Erro: -1
|*
	.global	_pause, _errno
 
PAUSE = 29

_pause:
	moveq	#PAUSE,d0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
