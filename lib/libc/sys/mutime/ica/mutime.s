|*
|****************************************************************
|*								*
|*			mutime.s				*
|*								*
|*	Obtem o tempo com microsegundos		 		*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: mutime						*
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
|*	mutime (mp)
|*	MUTM *mp;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mutime, _errno

MUTIME = 102

_mutime:
	movl	4(sp),a0		|* a0 = Endereco da variavel.
	moveq	#MUTIME,d0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
