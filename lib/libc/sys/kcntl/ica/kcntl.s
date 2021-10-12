|*
|****************************************************************
|*								*
|*			kcntl.s					*
|*								*
|*	Controle do KERNEL					*
|*								*
|*	Versão	1.0.0, de 02.02.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: kcntl						*
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
|*	kcntl (cmd, arg1, arg2)
|*	int   cmd;
|* 
|*	Retorno: Sucesso: 0; Erro: -1
|* 
	.global	_kcntl, _errno

KCNTL  =  106

_kcntl:
	moveq	#KCNTL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

