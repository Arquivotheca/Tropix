|*
|****************************************************************
|*								*
|*			getegid.s				*
|*								*
|*	Obtem a identificacao efetivo do grupo			*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getegid						*
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
|*	getegid ()
|*
|*	Retorna = Identificacao efetivo do grupo
|*
	.global	_getegid
 
GETGID = 47
 
_getegid:
	moveq	#GETGID,d0
	trap	#0
	movl	d1,d0
	rts
