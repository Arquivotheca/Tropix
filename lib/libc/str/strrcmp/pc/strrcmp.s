|*
|****************************************************************
|*								*
|*			strrcmp.s				*
|*								*
|*	Compara cadeias de caracteres (reverso)			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		3.0.0, de 10.08.95				*
|*								*
|*	Módulo: strrcmp						*
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
|*	Compara cadeias de caracteres (reverso)			*
|****************************************************************
|*
|*	int
|*	strrcmp (const char *str1, const char *str2)
|*
|*	devolve  +1, 0 ou -1  se "str1" é menor, igual ou maior "str2"
|*
	.global	_strrcmp
_strrcmp:
	movl	4(sp),r1
	movl	8(sp),r2

	cmpl	r1,r2		|* são uma mesma cadeia
	jeq	igual
loop:
	movb	(r1),r0
	cmpb	(r2),r0

	jlo	menor
	jhi	maior

	tstb	r0
	jz	igual

	incl	r1
	incl	r2

	jmp	loop
igual:
	clrl	r0
	rts
menor:
	movl	#1,r0		|* Reverso!
	rts
maior:
	movl	#-1,r0		|* Reverso!
	rts
