|*
|****************************************************************
|*								*
|*			uldiv.s					*
|*								*
|*	Divisão de dois inteiros longos sem sinal		*
|*								*
|*	Versão	1.0.0, de 01.12.86				*
|*		2.3.0, de 23.04.88				*
|*								*
|*	Módulo: crt						*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************

|*
|*	ulong		
|*	uldiv (a, b)		(a / b)
|*	ulong	a, b;
|*
	.global	uldiv, ulmul, ___udivsi3
uldiv:
___udivsi3:
	movw	4(sp),d0	|* se dividendo e
	orw	8(sp),d0	|* divisor < (2 ** 16)
	jne	1$

	movl	4(sp),d0	|* divisão curta
	divu	10(sp),d0
	andl	#0x0000FFFF,d0	|* apaga o resto da divisão
	rts

1$:
	moveml	#<d2,d3,d4>,-(sp)

	movl	16(sp),d0	|* carrega a
	movl	d0,d3
	movl	20(sp),d1	|* carrega b
	movl	d1,d4
	cmpl	#0x00010000,d1
	jhs	2$		|* divisor > (2 ** 16)

	clrw	d0		|* zera parte do dividendo
	swap	d0
	divu	d1,d0		|* a(alta) / b(baixa)
	movw	d0,d2
	movw	d3,d0		
	divu	d1,d0		|* resto e a(baixa) / b(baixa)
	swap	d0
	movw	d2,d0		|* ajeita o quociente
	swap	d0
	jra	4$
2$:
	lsrl	#1,d0		|* divide dividendo e
	lsrl	#1,d1		|* divisor por 2
	cmpl	#0x00010000,d1	|* até divisor < (2 ** 16)
	jhs	2$

	divu	d1,d0
	andl	#0x0000FFFF,d0	|* apaga o resto da divisão
	movl	d0,d2

	movl	d0,-(sp)	|* multiplica o quociente
	movl	d4,-(sp)	|* pelo divisor
	jsr	ulmul
	addql	#8,sp

	cmpl	d0,d3		|* compara com dividendo
	jhs	3$		|* se dividendo é menor então
	subql	#1,d2		|* subtrai um do quociente
3$:
	movl	d2,d0
4$:
	moveml	(sp)+,#<d2,d3,d4>
	rts
