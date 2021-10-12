|*
|****************************************************************
|*								*
|*			ldiv.s					*
|*								*
|*	Divisão de dois inteiros longos com sinal		*
|*								*
|*	Versão	1.0.0, de 01.12.86				*
|*		2.3.0, de 24.04.88				*
|*								*
|*	Módulo: crt						*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************

|*
|*	long
|*	ldiv (a, b)		(a / b)
|*	long	a, b;
|*
	.global	ldiv, ___divsi3, ulmul
ldiv:
___divsi3:
	movw	4(sp),d0	|* se dividendo e
	orw	8(sp),d0	|* divisor > 0 e < (2 ** 16)
	jne	1$

	movl	4(sp),d0	|* divisão curta
	divu	10(sp),d0
	andl	#0x0000FFFF,d0	|* apaga o resto da divisão
	rts
1$:
	moveml	#<d2,d3,d4,d5>,-(sp)

	movl	20(sp),d0	|* carrega dividendo
	movl	d0,d5
	jge	2$
	negl	d0
2$:
	movl	d0,d3
	movl	24(sp),d1	|* carrega divisor
	jge	3$
	negl	d1
	negl	d5
3$:
	movl	d1,d4
	cmpl	#0x00010000,d1
	jhs	4$		|* divisor maior que (2 ** 16)

	clrw	d0		|* zera parte alta do dividendo
	swap	d0
	divu	d1,d0		|* a(alta) / b(baixa)
	movw	d0,d2
	movw	d3,d0
	divu	d1,d0		|* resto e a(baixa) / b(baixa)
	swap	d0
	movw	d2,d0		|* ajeita o quociente
	swap	d0
	jra	6$
4$:
	lsrl	#1,d0		|* divide o dividendo e
	lsrl	#1,d1		|* o divisor por 2
	cmpl	#0x00010000,d1	|* até divisor < (2 ** 16)
	jhs	4$

	divu	d1,d0
	andl	#0x0000FFFF,d0	|* apaga o resto da divisão
	movl	d0,d2

	movl	d0,-(sp)	|* multiplica o quociente
	movl	d4,-(sp)	|* pelo divisor
	jsr	ulmul
	addql	#8,sp

	cmpl	d0,d3		|* compara com o dividendo
	jhs	5$		|* se dividendo é menor

	subql	#1,d2		|* subtrai um do quociente
5$:
	movl	d2,d0
6$:
	tstl	d5		|* ajusta o sinal
	jge	7$
	negl	d0
7$:
	moveml	(sp)+,#<d2,d3,d4,d5>
	rts
