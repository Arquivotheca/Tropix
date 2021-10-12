|*
|****************************************************************
|*								*
|*			lshlq.s					*
|*								*
|*	Deslocamento de 64 bits (lógico/arit para esquerda)	*
|*								*
|*	Versão	4.4.0, de 29.10.02				*
|*		4.4.0, de 29.10.02				*
|*								*
|*	Módulo: lshlq						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Deslocamento de 64 bits (lógico/arit para esquerda) *****
|*
|*	long long ___lshldi3  (long long shift, long long k);
|*	long long ___ashldi3  (long long shift, long long k);
|*
|*	Limitação: "k" até 32
|*
	.text
	.global ___lshldi3
	.global ___ashldi3
___lshldi3:
___ashldi3:
	movl	12(sp),r1

	movl	8(sp),r2
	movl	4(sp),r0
	lslq	r1,r0,r2

	lsll	r1,r0

	rts
