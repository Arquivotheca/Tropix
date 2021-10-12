|*
|****************************************************************
|*								*
|*			getscb.s				*
|*								*
|*	Obtem o bloco de controle do núcleo			*
|*								*
|*	Versão	1.0.0, de 03.02.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: getscb						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	SCB *
|*	getscb (sp)
|*	SCB *sp;
|*
|*	Retorna = Sucesso: &scb; Erro: -1
|*
	.global	_getscb, _errno

GETSCB = 99

_getscb:
	moveq	#GETSCB,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
