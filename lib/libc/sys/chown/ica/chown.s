|*
|****************************************************************
|*								*
|*			chown.s					*
|*								*
|*	Muda o dono e o grupo de um arquivo			*
|*								*
|*	Versão	1.0.0, de 08.01.86				*
|*		2.3.0, de 05.07.88				*
|*								*
|*	Modulo: chown						*
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
|*	chown (path, owner, group)
|*	char *path;
|*	int  owner;
|*	int  group;
|*
|*	Retorno = Sucesso: 0, Erro: -1
|*
	.global	_chown, _errno
 
CHOWN = 16
 
_chown:
	moveq	#CHOWN,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
