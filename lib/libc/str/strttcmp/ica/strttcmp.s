|*
|****************************************************************
|*								*
|*			strttcmp.s				*
|*								*
|*	Compara cadeias usando duas tabelas			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strttcmp					*
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
|*	Compara cadeias usando duas tabelas			*
|****************************************************************
|*
|*	int
|*	strttcmp (const char *s1, const char *s2,
|*					char table[], char tabled[])
|*
	.global	_strttcmp

_strttcmp:
	movl	a2,-4(sp)
desempate:
	movl	4(sp),a0	|* s1
	movl	8(sp),a1	|* s2
	movl	12(sp),a2	|* ender. da tabela

	cmpl	a0,a1 		|* São a mesma cadeia
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

	tstb	d1		|* Verifica se chegou no final da cadeia
	jne	loop

	movl	16(sp),d0	|* Desempata com a segunda tabela
	clrl	16(sp)

	movl	d0,12(sp) 	|* Se não tem tabela de desempate, é igual
	jne	desempate
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
