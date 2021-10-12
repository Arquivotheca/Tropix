|*
|****************************************************************
|*								*
|*			getgid.s				*
|*								*
|*	Obtem a identificacao real do grupo			*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getgid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	ushort
|*	getgid ()
|*
|*	Retorna = Identificacao real do grupo
|*
	.global	_getgid
 
GETGID = 47
 
_getgid:
	moveq	#GETGID,d0
	trap	#0
	rts
