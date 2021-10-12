|*
|****************************************************************
|*								*
|*			idcan.s					*
|*								*
|*	Converte um identificador para a forma canônica		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 02.12.88				*
|*								*
|*	Rotinas:						*
|*		idcan						*
|*								*
|*	Módulo: idcan						*
|*		libc/id						*
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
|*	idcan (id1, id2)
|*	idp_t	id1;
|*	char	*id2;
|*
	.global	_idcan

_idcan:
	movl	4(sp),a0	|* id1

	clrl	(a0)+		|* Zera os 16 bytes
	clrl	(a0)+
	clrl	(a0)+
	clrl	(a0)

	movl	4(sp),a0	|* id1
	movl	a0,d0
	movl	8(sp),a1	|* id2

	movw	#14-1,d1	|* Examina 14 caracteres
1$:
	movb	(a1)+,(a0)+	|* copia 1 byte

	dbeq	d1,1$

	rts
