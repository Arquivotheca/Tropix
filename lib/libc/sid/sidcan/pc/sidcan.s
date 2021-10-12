|*
|****************************************************************
|*								*
|*			sidcan.s				*
|*								*
|*	Converte um identificador curto para a forma canônica	*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 03.02.95				*
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
|*	Converte um identificador curto para a forma canônica	*
|****************************************************************
|*
|*	idp_t	sidcan (idp_t sid1, const char *id2);
|*
	.global	_sidcan
_sidcan:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* Zera os 8 bytes de "sid1"
	clrl	r0
	movl	r0,0(r5)
	movl	r0,4(r5)

   |***	movl	12(sp),r5	|* "sid1"
	movl	r5,r0		|* Retorna "sid1"
	movl	16(sp),r4	|* "sid2"

	movl	#7,r1		|* Examina 7 caracteres
   	up
1$:
	cmpb	#0,(r4)
	movsb
	loopnz	1$

	popl	r4
	popl	r5
	rts
