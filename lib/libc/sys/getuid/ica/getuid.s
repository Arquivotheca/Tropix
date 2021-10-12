|*
|****************************************************************
|*								*
|*			getuid.s				*
|*								*
|*	Obtem a identificação real do usuário			*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getuid						*
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
|*	getuid ()
|*
|*	Retorna = Identificacao real do usuario.
|*
	.global	_getuid
 
GETUID = 24
 
_getuid:
	moveq	#GETUID,d0
	trap	#0
	rts
