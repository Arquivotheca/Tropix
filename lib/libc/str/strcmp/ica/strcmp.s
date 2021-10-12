|*
|****************************************************************
|*								*
|*			strcmp.s				*
|*								*
|*	Compara cadeias						*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strcmp						*
|*		libc/str					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Compara cadeias						*
|****************************************************************
|*
|*	int
|*	strcmp (const char *src, const char *dst)
|*
|*	devolve  -1, 0 ou 1  se src é menor, igual ou maior dst
|*
	.global	_strcmp

_strcmp:
	movl	4(sp),a0
	movl	8(sp),a1

	cmpl	a0,a1		|* são uma mesma cadeia
	jeq	igual

loop:
	movb	(a0)+,d0
	cmpb	(a1)+,d0

	jlo	menor
	jhi	maior

	tstb	d0
	jne	loop
igual:
	moveq	#0,d0
	rts
menor:
	moveq	#-1,d0
	rts
maior:
	moveq	#1,d0
	rts
