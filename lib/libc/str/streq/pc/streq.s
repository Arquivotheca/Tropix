|*
|****************************************************************
|*								*
|*			streq.s					*
|*								*
|*	Compara cadeias						*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		3.0.0, de 03.01.95				*
|*								*
|*	Módulo: streq						*
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
|*	Compara cadeias de caracteres				*
|****************************************************************
|*
|*	int
|*	streq (const char *str1, const char *str2)
|*
|*	devolve  0 ou 1  se "str1" é diferente igual a "str2"
|*
	.global	_streq
_streq:
	movl	4(sp),r1
	movl	8(sp),r2

	cmpl	r1,r2		|* são uma mesma cadeia
	jeq	igual
loop:
	movb	(r1),r0
	cmpb	(r2),r0

	jne	diferente

	tstb	r0
	jz	igual

	incl	r1
	incl	r2

	jmp	loop
igual:
	movl	#1,r0
	rts

diferente:
	clrl	r0
	rts
