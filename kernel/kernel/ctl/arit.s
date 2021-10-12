|*
|****************************************************************
|*								*
|*			arit.s					*
|*								*
|*	Aritmética de 64 bits					*
|*								*
|*	Versão	4.4.0, de 28.10.02				*
|*		4.6.0, de 30.06.04				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Soma de 64 bits						*
|****************************************************************
|*
|*	long long  ___adddi3 (long long a, long long b);
|*
|*
	.text
	.global	___adddi3
___adddi3:
	movl	12(sp),r0
	addl	4(sp),r0

	movl	16(sp),r2
	adcl	8(sp),r2

	rts

.if 0	|*******************************************************|
|*
|****************************************************************
|*	Subtração de 64 bits					*
|****************************************************************
|*
|*	long long  ___subdi3 (long long a, long long b);
|*
	.text
	.global	___subdi3
___subdi3:
	movl	4(sp),r0
	subl	12(sp),r0

	movl	8(sp),r2
	sbbl	16(sp),r2

	rts
.endif	|*******************************************************|

|*
|****************************************************************
|*	Deslocamento de 64 bits (aritmético para direita)	*
|****************************************************************
|*
|*	long long ___ashrdi3  (long long shift, long long k);
|*
|*	Limitação: "k" até 32
|*
	.text
	.global ___ashrdi3
___ashrdi3:
	movl	12(sp),r1

	movl	8(sp),r2
	movl	4(sp),r0
	lsrq	r1,r2,r0

	asrl	r1,r2

	rts

|*
|****************************************************************
|*	Deslocamento de 64 bits (lógico/arit para esquerda)	*
|****************************************************************
|*
|*	long long ___lshldi3  (long long shift, long long k);
|*	long long ___ashldi3  (long long shift, long long k);
|*
|*	Limitação: "k" até 32
|*
	.text
|*	.global ___lshldi3
	.global ___ashldi3
___lshldi3:
___ashldi3:
	movl	12(sp),r1

	movl	8(sp),r2
	movl	4(sp),r0
	lslq	r1,r0,r2

	lsll	r1,r0

	rts

|*
|****************************************************************
|*	Divisão com sinal de 64 bits				*
|****************************************************************
|*
|*	long long ___divdi3 (long long numer, long long denom);
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

|*
|****************************************************************
|*	Regra de 3 com arredondamento (64 bits)			*
|****************************************************************
|*
|*	ulong mul_div_64 (ulong val, ulong num, ulong denom);
|*
|*	deslocamento:	     4		8	    12
|*
|*
	.text
	.global	_mul_div_64
_mul_div_64:
	movl	4(sp),r0		|* Multiplica 
	mulul	8(sp),r0

	movl	12(sp),r1		|* Prepara o arredondamento
	lsrl	#1,r1

	addl	r1,r0			|* Arredonda
	adcl	#0,r2

	divul	12(sp),r0		|* Divide

	rts
