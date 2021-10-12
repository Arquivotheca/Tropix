|*
|****************************************************************
|*								*
|*			fixunsdfsi.s				*
|*								*
|*	Obtém a parte inteira sem sinal de um número flutuante	*
|*								*
|*	Versão	3.0.0, de 15.07.95				*
|*		3.0.0, de 15.07.95				*
|*								*
|*	Módulo: fixdfsi						*
|*		/lib/libm486					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

ROUND	= 0

|*
|****************************************************************
|*	Obtém a parte inteira sem sinal de um número flutuante	*
|****************************************************************
|*
|*	int	__fixunsdfsi (double a);
|*
	.global	___fixunsdfsi
___fixunsdfsi:
	fldd	4(sp)

.if	ROUND
	fstcw	-12(sp)
	fldcw	trunc
.endif	ROUND

	fistpq	-8(sp)
	clrl	r0
	testb	#0x80,-1(sp)		|* Testa o sinal do número
	jnz	9$
	movl	-8(sp),r0
9$:
.if	ROUND
	fldcw	-12(sp)
.endif	ROUND
	rts

.if	ROUND
|*
|*	Constantes
|*
	.const
trunc:
	.word	0x0F7F
.endif	ROUND
