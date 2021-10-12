|*
|****************************************************************
|*								*
|*			stat.s					*
|*								*
|*	Obtem o estado de um arquivo				*
|*								*
|*	Versão	1.0.0, de 26.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: stat						*
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
|*	stat (path, buf)
|*	char *path;
|*	STAT *buf;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_stat, _errno
 
STAT = 18

_stat:
	moveq	#STAT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
