|*
|****************************************************************
|*								*
|*			ptrace.s				*
|*								*
|*	Executa os comandos de TRACE				*
|*								*
|*	Versão	1.0.0, de 22.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: ptrace						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	char *
|*	ptrace (req, pid, addr, data)
|*	int	req, pid, addr, data;
|*
|*	Retorna = Sucesso: Dado ou 0; Erro: -1
|*

	.global	_ptrace, _errno

PTRACE=	26

_ptrace:
	movl	d2,-(sp)

	moveq	#PTRACE,d0
	movl	8(sp),a0
	movl	12(sp),d1
	movl	16(sp),a1
	movl	20(sp),d2

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	movl	(sp)+,d2
	rts
