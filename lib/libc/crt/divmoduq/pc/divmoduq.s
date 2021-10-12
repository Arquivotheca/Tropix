|*
|****************************************************************
|*								*
|*			divmoduq.s				*
|*								*
|*   Divisão e resto sem sinal de 64 bits (divisor de 32 bits)	*
|*								*
|*	Versão	4.4.0, de 31.10.02				*
|*		4.4.0, de 31.10.02				*
|*								*
|*	Módulo: divmoduq					*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Divisão e resto sem sinal de 64 bits ********************
|*
|*	unsigned long _divmoduq (unsigned long long *numer, unsigned long denom);
|*
	.text
	.global	_divmoduq
_divmoduq:
	movl	4(sp),r1		|* Guarda o numerador
	movl	0(r1),r0
	movl	r0,-8(sp)
	movl	4(r1),r0
	movl	r0,-4(sp)

   |***	movl	-4(sp),r0		|* Guarda o quociente da parta alta
	clrl	r2
	divul	8(sp)
	movl	r0,-16(sp)

	mulul	8(sp),r0		|* Subtrai da parte alta
	subl	r0,-4(sp)

	movl	-4(sp),r2		|* Divide o restante
	movl	-8(sp),r0
	divul	8(sp)

	movl	r2,-12(sp)		|* Guarda o resto

	movl	r0,0(r1)		|* Armazena o quociente
	movl	-16(sp),r0
	movl	r0,4(r1)

	movl	-12(sp),r0		|* Retorna o resto

	rts
