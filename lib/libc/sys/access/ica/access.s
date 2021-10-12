|*
|****************************************************************
|*								*
|*			access.s				*
|*								*
|*	Informa sobre a permissão de acesso a um arquivo	*
|*								*
|*	Versão	1.0.0, de 05.01.87				*
|*		2.3.0, de 02.09.88				*
|*								*
|*	Módulo: access						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	ret = access (path, mode)
|*	char	*path;
|*	int	mode;
|*
|*	ret =  0 => Acesso é permitido
|*	ret = -1 => Nao é permitido,  ver errno.
|*
	.global	_access, _errno
 
ACCESS = 33

_access:
	moveq	#ACCESS,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
