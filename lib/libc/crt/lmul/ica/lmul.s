|*
|****************************************************************
|*								*
|*			lmul.s					*
|*								*
|*	Multiplicação de inteiros longos com sinal		*
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

|*
|*	long
|*	lmul (a, b)
|*	long	a, b;
|*
	.global	lmul, smul, ___mulsi3
lmul:
smul:
___mulsi3:
	movw	4(sp),d0	|* se a(alta) e
	orw	8(sp),d0	|* b(alta) são nulos
	jne	1$

	movw	6(sp),d0	|* multiplicação curta
	mulu	10(sp),d0
	rts

1$:
	moveml	#<d2,d3,d4>,-(sp)		

	movl	16(sp),d2	|* carrega a
	movl	d2,d4		|* guarda sinal
	jge	2$
	negl	d2		|* a é negativo
2$:
	movl	20(sp),d3	|* carrega b
	jge	3$
	negl	d3		|* b é negativo
	negl	d4		|* complementa sinal
3$:
	movw	d2,d0
	mulu	d3,d0		|* b(baixo) * a(baixo)

	movl	d2,d1
	orl	d3,d1
	swap	d1
	tstw	d1
	jeq	4$		|* a e b são < (2 ** 16)

	movw	d2,d1
	swap	d2
	mulu	d3,d2		|* b(baixo) * a(alto) 
	swap	d3
	mulu	d3,d1		|* b(alto) * a(baixo)
	addl	d2,d1		|* soma partes altas
	swap	d1
	clrw	d1		|* joga fora 16 bits
	addl	d1,d0		|* soma parte baixa
4$:
	tstl	d4		|* ajusta o sinal
	jge	5$
	negl	d0
5$:
	moveml	(sp)+,#<d2,d3,d4>
	rts
