|*
|****************************************************************
|*								*
|*			chdir.s					*
|*								*
|*	Troca de diretório corrente				*
|*								*
|*	Versão	1.0.0, de 08.01.86				*
|*		2.3.0, de 05.07.88				*
|*								*
|*	Modulo: chdir						*
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
|*	chdir (path)
|*	char *path;
|*
|*	Retorna = Sucesso: 0, erro: -1
|*
	.global	_chdir, _errno
 
CHDIR = 12
 
_chdir:
	moveq	#CHDIR,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
