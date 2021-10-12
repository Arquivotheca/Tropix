|*
|****************************************************************
|*								*
|*			subq.s					*
|*								*
|*	Subtração de 64 bits					*
|*								*
|*	Versão	4.4.0, de 29.10.02				*
|*		4.4.0, de 29.10.02				*
|*								*
|*	Módulo: subq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Subtração de 64 bits ************************************
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
