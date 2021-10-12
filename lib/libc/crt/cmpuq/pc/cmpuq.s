|*
|****************************************************************
|*								*
|*			cmpuq.s					*
|*								*
|*	Comparação sem sinal de 64 bits				*
|*								*
|*	Versão	4.4.0, de 31.10.02				*
|*		4.4.0, de 31.10.02				*
|*								*
|*	Módulo: cmpuq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Comparação sem sinal de 64 bits	*************************
|*
|*	int _cmpuq (unsigned long long a, unsigned long long b);
|*
	.text
	.global	___ucmpdi2
___ucmpdi2:
	movl	#1,r0

	movl	16(sp),r2
	cmpl	8(sp),r2
	jls	2$
1$:
	decl	r0			|* A parte é maior
	rts
2$:
	jeq	5$			|* Desvia se a parte alta é igual
3$:
	incl	r0			|* A parte é menor
	rts
5$:
	movl	12(sp),r2		|* As partes altas são iguais
	cmpl	4(sp),r2		|* Compara as partes baixas
	jhi	1$

	jne	3$			|* Desvia se a parte baixa é diferente

	rts				|* Igual: retorna com 1
