|*
|****************************************************************
|*								*
|*			geteuid.s				*
|*								*
|*	Obtem a identificacao efetiva do usuario		*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: geteuid						*
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
|*	geteuid ()
|*
|*	Retorna = Identificação efetiva do usuário.
|*
	.global	_geteuid
 
GETUID = 24
 
_geteuid:
	moveq	#GETUID,d0
	trap	#0
	movl	d1,d0
	rts
