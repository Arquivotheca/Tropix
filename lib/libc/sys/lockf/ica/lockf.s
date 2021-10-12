|*
|****************************************************************
|*								*
|*			lockf.s					*
|*								*
|*	Gerencia LOCKs em regiões de arquivos			*
|*								*
|*	Versão	2.1.0, de 04.08.88				*
|*		2.3.0, de 04.08.88				*
|*								*
|*	Modulo: lockf 						*
|*		libc/sys 					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	int
|*	lockf (fd, mode, size)
|*	int	fd;
|*	int	mode;
|*	long	size;
|*
|*	Retorno: Sucesso: 0; Erro: -1
|*
	.global	_lockf, _errno

LOCKF = 56

_lockf:
	moveq	#LOCKF,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
