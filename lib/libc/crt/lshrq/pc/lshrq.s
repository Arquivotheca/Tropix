|*
|****************************************************************
|*								*
|*			lshrq.s					*
|*								*
|*	Deslocamento de 64 bits (lógico para direita)		*
|*								*
|*	Versão	4.4.0, de 29.10.02				*
|*		4.4.0, de 29.10.02				*
|*								*
|*	Módulo: lshrq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Deslocamento de 64 bits (lógico para direita) ***********
|*
|*	long long ___lshrdi3  (long long shift, long long k);
|*
|*	Limitação: "k" até 32
|*
	.text
	.global ___lshrdi3
___lshrdi3:
	movl	12(sp),r1

	movl	8(sp),r2
	movl	4(sp),r0
	lsrq	r1,r2,r0

	lsrl	r1,r2

	rts
