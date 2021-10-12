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
|*	Módulo: xc/lib/aux					*
|*		X Windows - Bibliotecas				*
|*		Baseado no software homônimo do XFree86		*
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
