|*
|****************************************************************
|*								*
|*			mkdir.s					*
|*								*
|*	Cria um diretório					*
|*								*
|*	Versão	3.0.0, de 18.11.92				*
|*		3.0.0, de 18.11.92				*
|*								*
|*	Modulo: mkdir						*
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
|*	mkdir (char *path, int mode)
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mkdir, _errno
 
MKDIR = 75

_mkdir:
	moveq	#MKDIR,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
