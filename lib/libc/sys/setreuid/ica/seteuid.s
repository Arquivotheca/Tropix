|*
|****************************************************************
|*								*
|*			seteuid.s				*
|*								*
|*	Estabelece o EUID do processo				*
|*								*
|*	Versão	3.0.0, de 02.11.92				*
|*		3.0.0, de 02.11.92				*
|*								*
|*	Modulo: seteuid						*
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
|*	seteuid (euid)
|*	int euid;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_seteuid, _errno
 
SETREUID = 73

_seteuid:
	moveq	#SETREUID,d0
	movw	#-1,a0		|* ruid
	movl	4(sp),d1	|* euid

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
