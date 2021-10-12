|*
|****************************************************************
|*								*
|*			delay.s					*
|*								*
|*	Espera um número determinado de microsegundos		*
|*								*
|*	Versão	4.0.0, de 18.06.00				*
|*		4.0.0, de 03.07.00				*
|*								*
|*	Módulo: Boot2						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*


|*
|****************************************************************
|*	Espera um número determinado de microsegundos		*
|****************************************************************
|*
	.text
	.global	_DELAY
_DELAY:
	movl	4(sp),r2		|* O No. de useg
	movl	_DELAY_value,r1		|* O Valor para 1 useg
1$:
	subl	#1,r2
	jlt	9$

	movl	r1,r0
3$:
	subl	#1,r0
	jge	3$

	jmp	1$
9$:
	rts

|*
|****************************************************************
|*	Le o TSC						*
|****************************************************************
|*
	.text
	.global	_read_TSC
_read_TSC:
	rdtsc
	rts

|*
|****************************************************************
|*	Regra de 3 com arredondamento (64 bits)			*
|****************************************************************
|*
|*	ulong mul_div_64 (ulong val, ulong num, ulong denom);
|*
|*	deslocamento:	     4		8	    12
|*
|*
	.text
	.global	_mul_div_64
_mul_div_64:
	movl	4(sp),r0		|* Multiplica 
	mulul	8(sp),r0

	movl	12(sp),r1		|* Prepara o arredondamento
	lsrl	#1,r1

	addl	r1,r0			|* Arredonda
	adcl	#0,r2

	divul	12(sp),r0		|* Divide

	rts
