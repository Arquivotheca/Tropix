|*
|****************************************************************
|*								*
|*			orq.s					*
|*								*
|*	Soma de 64 bits						*
|*								*
|*	Versão	4.6.0, de 29.05.04				*
|*		4.6.0, de 29.05.04				*
|*								*
|*	Módulo: orq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Ou de 64 bits *******************************************
|*
|*	long long ___iordi3 (long long a, long long b);
|*
	.text
	.global	___iordi3
___iordi3:
	movl	12(sp),r0
	orl	4(sp),r0

	movl	16(sp),r2
	orl	8(sp),r2

	rts
