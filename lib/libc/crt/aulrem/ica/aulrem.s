|*
|****************************************************************
|*								*
|*			aulrem.s				*
|*								*
|*	Resto da divisão de dois inteiros longos  sem sinal	*
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
|*	ulong
|*	aulrem (&a, b)		resto de (a / b)
|*	long	a, b;
|*
	.global	aulrem, ulmul
aulrem:
	movl	4(sp),a0	|* end. do dividendo em a0
	movw	(a0),d0		|* se dividendo e
	orw	8(sp),d0	|* divisor < (2 ** 16)
	jne	1$

	movl	(a0),d0		|* divisão curta
	divu	10(sp),d0
	clrw	d0		|* zera quociente
	swap	d0
	movl	d0,(a0)
	rts
1$:
	moveml	#<d2,d3>,-(sp)

	movl	(a0),d0		|* carrega dividendo
	movl	d0,d2
	movl	16(sp),d1	|* carrega divisor
	cmpl	#0x00010000,d1
	jhs	2$		|* divisor > (2 ** 16)

	clrw	d0
	swap	d0
	divu	d1,d0		|* a(alta) / b(baixa)
	movw	d2,d0
	divu	d1,d0		|* resto e a(baixa) / b(baixa)
	clrw	d0		|* zera quociente
	swap	d0		|* posiciona resto
	jra	5$
2$:
	movl	d1,d3
3$:
	lsrl	#1,d0		|* divide dividendo e
	lsrl	#1,d1		|* divisor por 2 até que
	cmpl	#0x00010000,d1	|* divisor < (2 ** 16)
	jhs	3$

	divu	d1,d0
	andl	#0x0000FFFF,d0

	movl	d0,-(sp)	|* multiplica quociente
	movl	d3,-(sp)	|* pelo divisor
	jsr	ulmul
	addql	#8,sp

	cmpl	d0,d2		|* se resultado > dividendo
	jhs	4$
	subl	d3,d0		|* então (resultado - divisor)
4$:
	subl	d2,d0		|*  (resultado - dividendo)
	negl	d0
5$:
	movl	d0,(a0)
	moveml	(sp)+,#<d2,d3>
	rts
