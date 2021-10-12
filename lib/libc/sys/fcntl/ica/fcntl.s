|*
|****************************************************************
|*								*
|*			fcntl.s					*
|*								*
|*	Controle de arquivos					*
|*								*
|*	Versão	1.0.0, de 14.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: fcntl						*
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
|*	fcntl (fd, cmd, arg)
|*	int    fd, cmd, arg;
|*
|*	Retorno = Sucesso: 0 ou > 0; Erro: -1
|*
	.global	_fcntl, _errno

FCNTL = 65

_fcntl:
	moveq	#FCNTL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
