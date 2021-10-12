|*
|****************************************************************
|*								*
|*			setruid.s				*
|*								*
|*	Estabelece o RUID do processo				*
|*								*
|*	Versão	3.0.0, de 02.11.92				*
|*		3.0.0, de 02.11.92				*
|*								*
|*	Modulo: setruid						*
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
|*	setruid (ruid)
|*	int ruid;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setruid, _errno
 
SETREUID = 73

_setruid:
	moveq	#SETREUID,d0
	movl	4(sp),a0	|* ruid
	moveq	#-1,d1		|* euid

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
