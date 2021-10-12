|*
|****************************************************************
|*								*
|*			uname.s					*
|*								*
|*	Obtem a identificação do sistema			*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: uname						*
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
|*	uname (name)
|*	UTSNAME *name;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_uname, _errno

UNAME =	71

_uname:
	moveq	#UNAME,d0	
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
