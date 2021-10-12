|*
|****************************************************************
|*								*
|*			link.s					*
|*								*
|*	Cria uma entrada de diretório				*
|*								*
|*	Versão	1.0.0, de 19.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: link						*
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
|*	link (path1, path2)
|*	char *path1, *path2;
|*
|*	Retorno = Sucesso: 0; Erro: -1
|*
	.global	_link, _errno
 
LINK = 9
 
_link:
	moveq	#LINK,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
