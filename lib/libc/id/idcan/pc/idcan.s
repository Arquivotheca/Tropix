|*
|****************************************************************
|*								*
|*			idcan.s					*
|*								*
|*	Converte um identificador para a forma canônica		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 06.01.95				*
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
|*	idp_t	idcan (idp_t id1, const char *id2);
|*
	.global	_idcan
_idcan:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* Zera os 16 bytes de "id1"
	movl	#4,r1
	clrl	r0
	up
	rep
	stosl

	movl	12(sp),r5	|* "id1"
	movl	r5,r0		|* Retorna "id1"
	movl	16(sp),r4	|* "id2"

	movl	#14,r1		|* Examina 14 caracteres
   |***	up
1$:
	cmpb	#0,(r4)
	movsb
	loopnz	1$

	popl	r4
	popl	r5
	rts
