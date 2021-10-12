|*
|****************************************************************
|*								*
|*			stricmp.s				*
|*								*
|*	Compara cadeias em código ISO				*
|*								*
|*	Versão	1.0.0, de 07.02.87				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: stricmp						*
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
|*	Compara cadeias em código ISO				*
|****************************************************************
|*
|*	int
|*	stricmp (const char *src, const char *dst)
|*
	.global	_stricmp, _cmpisotb

_stricmp:
	movl	4(sp),a0
	movl	8(sp),a1

	movl	a2,-4(sp)
	movl	#_cmpisotb,a2	|* ender. da tabela

	cmpl	a0,a1		|* são a mesma cadeia?
	jeq	igual

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
