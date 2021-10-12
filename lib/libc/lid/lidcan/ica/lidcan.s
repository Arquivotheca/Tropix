|*
|****************************************************************
|*								*
|*			lidcan.s				*
|*								*
|*	Converte um identificador longo para a forma canônica	*
|*								*
|*	Versão	2.1.0, de 19.01.89				*
|*								*
|*	Rotinas:						*
|*		lidcan						*
|*								*
|*	Módulo: lidcan						*
|*		libc/lid					*
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
|*	lidcan (lid1, lid2)
|*	idp_t	lid1;
|*	char	*lid2;
|*
	.global	_lidcan

_lidcan:
	movl	4(sp),a0	|* lid1

	clrl	(a0)+		|* Zera os 32 bytes
	clrl	(a0)+
	clrl	(a0)+
	clrl	(a0)+

	clrl	(a0)+
	clrl	(a0)+
	clrl	(a0)+
	clrl	(a0)

	movl	4(sp),a0	|* lid1
	movl	a0,d0
	movl	8(sp),a1	|* lid2

	movw	#31-1,d1	|* Examina 31 caracteres
1$:
	movb	(a1)+,(a0)+	|* copia 1 byte

	dbeq	d1,1$

	rts
