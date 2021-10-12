|*
|****************************************************************
|*								*
|*			setgid.s				*
|*								*
|*	Atribui o GID do processo				*
|*								*
|*	Versão	3.0.0, de 09.02.95				*
|*		3.0.0, de 09.02.95				*
|*								*
|*	Módulo: setgid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETGID		= 46
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "setgid" *****************************
|*
|*	int	setgid (int gid);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setgid
_setgid:
	movl	#SETGID,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
