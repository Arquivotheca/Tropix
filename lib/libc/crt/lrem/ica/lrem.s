|*
|****************************************************************
|*								*
|*			lrem.s					*
|*								*
|*	Resto da divisão de inteiros longos com sinal		*
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
|*	lrem (a, b)		resto de (a / b) com sinal de a
|*	long	a, b;
|*
	.globl	lrem, ___modsi3, ulmul
lrem:
___modsi3:
	movw	4(sp),d0	|* se dividendo e
	orw	8(sp),d0	|* divisor > 0 e < (2 ** 16)
	jne	1$

	movl	4(sp),d0	|* divisão curta
	divu	10(sp),d0
	clrw	d0		|* zera quociente
	swap	d0
	rts

1$:
	moveml	#<d2,d3,d4>,-(sp)		

	movl	16(sp),d0	|* carrega dividendo
	movl	d0,d4		|* carrega sinal
	jge	2$
	negl	d0
2$:
	movl	d0,d2
	movl	20(sp),d1	|* carrega divisor
	jge	3$
	negl	d1
3$:
	cmpl	#0x00010000,d1
	jhs	4$		|* divisor > (2 ** 16)

	clrw	d0
	swap	d0
	divu	d1,d0		|* a(alta) / b(baixa)
	movw	d2,d0
	divu	d1,d0		|* resto e a(baixa) / b (baixa)
	clrw	d0
	swap	d0		|* posiciona o resto
	jra	7$
4$:
	movl	d1,d3		|* guarda divisor
5$:
	lsrl	#1,d0		|* divide dividendo e
	lsrl	#1,d1		|* divisor por 2 até que
	cmpl	#0x00010000,d1	|* divisor < (2 ** 16)
	jhs	5$

	divu	d1,d0
	andl	#0x0000FFFF,d0

	movl	d0,-(sp)	|* multiplica quociente
	movl	d3,-(sp)	|* pelo divisor
	jsr	ulmul
	addql	#8,sp

	cmpl	d0,d2		|* se resultado > dividendo
	jhs	6$
	subl	d3,d0		|* então (resultado - divisor)
6$:
	subl	d2,d0		|* (resultado - dividendo)
	negl	d0
7$:
	tstl	d4		|* sinal do resultado
	jge	8$
	negl	d0
8$:
	moveml	(sp)+,#<d2,d3,d4>
	rts
