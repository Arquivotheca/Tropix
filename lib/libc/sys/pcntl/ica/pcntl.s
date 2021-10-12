|*
|****************************************************************
|*								*
|*			pcntl.s					*
|*								*
|*	Controle dos Processos e Threads			*
|*								*
|*	Versão	1.0.0, de 03.07.91				*
|*		2.3.0, de 03.07.91				*
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
|*	pcntl (cmd, arg1, arg2)
|*	int   cmd;
|* 
|*	Retorno: Sucesso: 0; Erro: -1
|* 
	.global	_pcntl, _errno

PCNTL  =  120

_pcntl:
	moveq	#PCNTL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

