|*
|****************************************************************
|*								*
|*			dup.s					*
|*								*
|*	Duplica um descritor de arquivos			*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: dup						*
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
|*	dup (fd1)
|*	int fd1;
|*
|*	Retorno = Sucesso: fd >= 0; Erro: -1
|*
|*	int
|*	dup2 (fd1, fd2)
|*	int fd1;
|*	int fd2;
|*
|*	Retorno = Sucesso: fd >= 0; Erro: -1
|*
	.global	_dup, _dup2, _errno
 
DUP = 41 

_dup2:
	orl	#0x40,4(sp)
 
_dup:
	moveq	#DUP,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
