|*
|****************************************************************
|*								*
|*			wait.s					*
|*								*
|*	Espera um filho terminar				*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 08.04.95				*
|*								*
|*	Módulo: wait						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

WAIT		= 7
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "wait" *******************************
|*
|*	int	wait (int *status)
|*
|*	Retorna = Sucesso: ID do filho; Erro: -1
|*
|*	Repare que o argumento "status" não é recebido pelo KERNEL!
|*
	.global	_wait
_wait:
	pushl	r4
	pushl	r3

	movl	#WAIT,r0
	callgl	#SYS_CALL,0

	tstl	r2		|* Verifica se houve erro
	jz	2$

	popl	r3
	popl	r4

	jmp	syscall_error
2$:
	movl	r3,__faddr
	movl	r4,__fpc

	popl	r3
	popl	r4

	movl	4(sp),r2
	tstl	r2
	jz	4$

	movl	r1,(r2)		|* Armazena o "status"
4$:
	rts

|*
|*	Endereço de êrro
|*
	.bss
	.global	__faddr, __fpc
__faddr:
	.long	0
__fpc:
	.long	0
