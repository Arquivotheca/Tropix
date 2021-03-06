|*
|****************************************************************
|*								*
|*			modsq.s					*
|*								*
|*	Resto com sinal de 64 bits 				*
|*								*
|*	Vers?o	4.4.0, de 03.11.02				*
|*		4.4.0, de 03.11.02				*
|*								*
|*	M?dulo: modsq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright ? 2002 NCE/UFRJ - tecle "man licen?a"	*
|* 								*
|****************************************************************
|*

|*
|******	Resto com sinal de 64 bits ******************************
|*
|*	long long modsq (long long numer, long long denom);
|*
	.text
	.global	___moddi3
___moddi3:
	clrl	-8(sp)			|* O sinal da divis?o

	cmpl	#0,8(sp)
	jpl	1$

	notl	8(sp)
	notl	4(sp)
	addl	#1,4(sp)

	eorl	#1,-8(sp)
1$:
	cmpl	#0,16(sp)
	jpl	2$

	notl	16(sp)
	notl	12(sp)
	addl	#1,12(sp)

   |***	eorl	#1,-8(sp)
2$:
	movl	16(sp),r0
	tstl	r0
	jnz	5$

|******	O divisor ? de 32 bits **********************************

	movl	8(sp),r0
	clrl	r2
	divul	12(sp)

	mulul	12(sp),r0
	subl	r0,8(sp)

	movl	8(sp),r2
	movl	4(sp),r0
	divul	12(sp)

	movl	r2,r0
	clrl	r2

	jmp	20$

|******	O divisor ? de 64 bits **********************************

5$:
	movl	r3,-4(sp)

	bsrl	r0,r1			|* Calcula o deslocamento necess?rio
	incl	r1

	movl	16(sp),r2		|* Desloca o divisor
	movl	12(sp),r0
	lsrq	r1,r2,r0
	movl	r0,r3			|* Guarda a parte baixa

	movl	8(sp),r2		|* Desloca o dividendo
	movl	4(sp),r0
	lsrq	r1,r2,r0
	lsrl	r1,r2

	divul	r3			|* Divis?o aproximada
	movl	r0,r3

	movl	16(sp),r0		|* Multiplica de volta
	mulul	r3,r0
	movl	r0,r1

	movl	12(sp),r0
	mulul	r3,r0
	addl	r1,r2

	subl	4(sp),r0		|* Subtrai do dividendo
	sbbl	8(sp),r2

	notl	r0			|* Inverte o sinal
	notl	r2

	addl	#1,r0
	adcl	#0,r2

	jpl	9$

	addl	12(sp),r0		|* Passou, corrige
	adcl	16(sp),r2
9$:
	movl	-4(sp),r3

|*****	Acerta o sinal ******************************************

20$:
	cmpl	#0,-8(sp)
	jnz	25$

	rts
25$:
	notl	r2
	notl	r0
	addl	#1,r0

	rts
