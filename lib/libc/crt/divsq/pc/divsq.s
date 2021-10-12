|*
|****************************************************************
|*								*
|*			divsq.s					*
|*								*
|*	Divisão com sinal de 64 bits 				*
|*								*
|*	Versão	4.4.0, de 03.11.02				*
|*		4.4.0, de 03.11.02				*
|*								*
|*	Módulo: divsq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Divisão com sinal de 64 bits ****************************
|*
|*	long long divsq (long long numer, long long denom);
|*
	.text
	.global	___divdi3
___divdi3:
	clrl	-8(sp)			|* O sinal da divisão

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

	eorl	#1,-8(sp)
2$:
	movl	16(sp),r0
	tstl	r0
	jnz	5$

|******	O divisor é de 32 bits **********************************

	movl	8(sp),r0		|* Põe em r1 o quociente da parta alta
	clrl	r2
	divul	12(sp)
	movl	r0,r1

	mulul	12(sp),r0
	subl	r0,8(sp)

	movl	8(sp),r2
	movl	4(sp),r0
	divul	12(sp)

	movl	r1,r2

	jmp	20$

|******	O divisor é de 64 bits **********************************

5$:
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

	cmpl	8(sp),r2		|* Compara
	jhi	7$

	cmpl	4(sp),r0
	jls	9$
7$:
	decl	r3
9$:
	clrl	r2			|* O quociente
	movl	r3,r0

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
