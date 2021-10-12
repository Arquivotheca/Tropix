|*
|****************************************************************
|*								*
|*			divuq.s					*
|*								*
|*	Divisão sem sinal de 64 bits 				*
|*								*
|*	Versão	4.4.0, de 29.10.02				*
|*		4.4.0, de 29.10.02				*
|*								*
|*	Módulo: divuq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Divisão sem sinal de 64 bits ****************************
|*
|*	unsigned long long divuq (unsigned long long numer, unsigned long long denom);
|*
	.text
	.global	___udivdi3
___udivdi3:
	movl	16(sp),r0
	tstl	r0
	jnz	1$

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

	rts
