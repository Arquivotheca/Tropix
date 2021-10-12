|*
|****************************************************************
|*								*
|*			strtcmp.s				*
|*								*
|*	Compara cadeias usando uma tabela			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strtcmp						*
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
|*	Compara cadeias usando uma tabela			*
|****************************************************************
|*
|*	int
|*	strtcmp (const char *src, const char *dst, char table[])
|*
	.global	_strtcmp

_strtcmp:
	movl	4(sp),a0
	movl	8(sp),a1

	movl	a2,-4(sp)
	movl	12(sp),a2	|* ender. da tabela

	cmpl	a0,a1
	jeq	igual		|* são a mesma cadeia

	moveq	#0,d0
	moveq	#0,d1

loop:
	movb	(a0)+,d0
	movb	(a1)+,d1

	movb	0(a2,d0),d0	|* indexa a tabela
	cmpb	0(a2,d1),d0

	jlo	menor
	jhi	maior

	tstb	d1
	jne	loop

	movl	-4(sp),a2
	jmp	_strcmp		|* desempata com ASCII

igual:
	moveq	#0,d0
	jra	exit
menor:
	moveq	#-1,d0
	jra	exit
maior:
	moveq	#1,d0
exit:
	movl	-4(sp),a2
	rts
