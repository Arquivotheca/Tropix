|*
|****************************************************************
|*								*
|*			getpid.s				*
|*								*
|*	Obtem a identifica??o do processo			*
|*								*
|*	Vers?o	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getpid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright ? 2000 NCE/UFRJ - tecle "man licen?a"	*
|* 								*
|****************************************************************
|*

|*
|*	int
|*	getpid ()
|*
|*	Retorna = identificacao do processo
|*
	.global	_getpid
 
GETPID = 20
 
_getpid:
	moveq	#GETPID,d0
	trap	#0
	rts
