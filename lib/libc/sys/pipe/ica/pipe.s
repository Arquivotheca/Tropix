|*
|****************************************************************
|*								*
|*			pipe.s					*
|*								*
|*	Cria um canal entre processos				*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: pipe						*
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
|*	pipe (fd)
|*	int fd[2];
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_pipe, _errno

PIPE = 42

_pipe:
	moveq	#PIPE,d0

	trap	#0
	jcs	erro

	movl	4(sp),a0
	movl	d0,(a0)+
	movl	d1,(a0)

	clrl	d0
	rts

erro:
	movl	d0,_errno
	moveq	#-1,d0
	rts
