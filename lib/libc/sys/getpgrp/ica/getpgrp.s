|*
|****************************************************************
|*								*
|*			getpgrp.s				*
|*								*
|*	Obtem a identificacao do grupo de processos		*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getpgrp						*
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
|*	getpgrp ()
|*
|*	Retorna a identificacao do grupo
|*
	.global	_getpgrp

GETPGRP = 66

_getpgrp:
	moveq	#GETPGRP,d0
	trap	#0
	rts
