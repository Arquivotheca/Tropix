|*
|****************************************************************
|*								*
|*			untext.s				*
|*								*
|*	Desaloca um "text" ativo				*
|*								*
|*	Versão	1.0.0, de 29.06.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: untext						*
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
|*	untext (path)
|*	char *path;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_untext, _errno
 
UNTEXT = 107

_untext:
	moveq	#UNTEXT,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
