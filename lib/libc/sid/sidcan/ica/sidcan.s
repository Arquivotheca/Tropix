|*
|****************************************************************
|*								*
|*			sidcan.s				*
|*								*
|*	Converte um identificador para a forma canônica		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 17.04.89				*
|*								*
|*	Rotinas:						*
|*		sidcan						*
|*								*
|*	Módulo: sidcan						*
|*		libc/sid					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Converte no Formato Canonico				*
|****************************************************************
|*
|*	idp_t
|*	sidcan (id1, id2)
|*	idp_t	id1;
|*	char	*id2;
|*
	.global	_sidcan

_sidcan:
	movl	4(sp),a0	|* id1

	clrl	(a0)+		|* Zera os 8 bytes
	clrl	(a0)

	movl	4(sp),a0	|* id1
	movl	a0,d0
	movl	8(sp),a1	|* id2

	movw	#7-1,d1		|* Examina 7 caracteres
1$:
	movb	(a1)+,(a0)+	|* copia 1 byte

	dbeq	d1,1$

	rts
