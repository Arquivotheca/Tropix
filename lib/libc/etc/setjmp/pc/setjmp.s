|*
|****************************************************************
|*								*
|*			setjmp.s				*
|*								*
|*	"Goto" Não local					*
|*								*
|*	Versão	1.0.0, de 11.11.86				*
|*		3.0.0, de 03.01.95				*
|*								*
|*	Módulo: setjmp						*
|*		libc/etc					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

 
|*
|****************************************************************
|*	Prepara o desvio					*
|****************************************************************
|*
	.global	_setjmp
_setjmp:
	movl	4(sp),r1		|* r1 = env

	movl	(sp),r0			|* Endereço de retorno
	movl	r0,0(r1)

	movl	r3,4(r1)		|* Guarda os registros
	movl	r4,8(r1)
	movl	r5,12(r1)
	movl	fp,16(r1)
	movl	sp,20(r1)

	clrl	r0			|* Retorna com 0
	rts

|*
|****************************************************************
|*	Desvia							*
|****************************************************************
|*
	.global	_longjmp
_longjmp:
	movl	4(sp),r1		|* r1 = env

	movl	8(sp),r0		|* r0 = algo NÃO-zero
	tstl	r0
	jnz	1$

	movl	#1,r0
1$:
	movl	4(r1),r3		|* Restaura os registros
	movl	8(r1),r4
	movl	12(r1),r5
	movl	16(r1),fp
	movl	20(r1),sp

	popl	r2			|* Retorna
	jmp	0(r1)
