|*
|****************************************************************
|*								*
|*			moduq.s					*
|*								*
|*	Resto sem sinal de 64 bits 				*
|*								*
|*	Versão	4.4.0, de 03.11.02				*
|*		4.4.0, de 03.11.02				*
|*								*
|*	Módulo: moduq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Resto sem sinal de 64 bits ******************************
|*
|*	unsigned long long moduq (unsigned long long numer, unsigned long long denom);
|*
	.text
	.global	___umoddi3
___umoddi3:
	movl	16(sp),r0
	tstl	r0
	jnz	1$

|******	O divisor é de 32 bits **********************************

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

	rts

|******	O divisor é de 64 bits **********************************

1$:
	movl	r3,-4(sp)

	bsrl	r0,r1			|* Calcula o deslocamento necessário
	incl	r1

	movl	16(sp),r2		|* Desloca o divisor
	movl	12(sp),r0
	lsrq	r1,r2,r0
	movl	r0,r3			|* Guarda a parte baixa

	movl	8(sp),r2		|* Desloca o dividendo
	movl	4(sp),r0
	lsrq	r1,r2,r0
	lsrl	r1,r2

	divul	r3			|* Divisão aproximada
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

	rts
