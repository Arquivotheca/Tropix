|*
|****************************************************************
|*								*
|*			open.s					*
|*								*
|*	Abre um descritor de arquivos				*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: open						*
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
|*	open (path, oflag, mode)
|*	char *path;
|*	int  oflag, mode;
|*
|*	Retorna = Sucesso: fd (>= 0); Erro: -1
|*
	.global	_open, _errno
 
OPEN = 5

_open:
	moveq	#OPEN,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
