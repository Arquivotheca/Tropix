|*
|****************************************************************
|*								*
|*			wait.s					*
|*								*
|*	Espera um filho terminar				*
|*								*
|*	Versão	1.0.0, de 30.01.87				*
|*		2.3.0, de 01.03.87				*
|*								*
|*	Modulo: wait						*
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
|*	wait (&status)
|*	int *status;
|*
|*	Retorna = Sucesso: ID do filho; Erro: -1
|*
	.global	_wait, _errno, __faddr, __fpc
 
WAIT = 7

_wait:
	moveq	#WAIT,d0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
	rts
1$:
	movl	a0,__faddr
	movl	a1,__fpc

	tstl	4(sp)
	jeq	2$

	movl	4(sp),a0
	movl	d1,(a0)
2$:
	rts

|*
|*	Endereco de Erro
|*
	.bss

__faddr:
	.long	0
__fpc:
	.long	0
