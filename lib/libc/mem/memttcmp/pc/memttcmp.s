|*
|****************************************************************
|*								*
|*			memttcmp.s				*
|*								*
|*	Compara áreas de memoria dadas duas tabelas		*
|*								*
|*	Versão	3.0.0, de 07.07.95				*
|*		3.0.0, de 11.08.95				*
|*								*
|*	Módulo: memttcmp					*
|*		libc/mem					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Compara áreas de memoria dadas duas tabelas		*
|****************************************************************
|*
|*	void	*memttcmp (const void *dst, const void *src, size_t count,
|*					const char tb[], const char tba[]);
|*
	.global	_memttcmp
_memttcmp:
	link	#0
	pushl	r5
	pushl	r4
	pushl	r3

	movl	20(fp),r3	|* "tb"
1$:
	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"
   |***	movl	20(fp),r3	|* "tb"

	cmpl	r4,r5
	jeq	8$

	clrl	r0		|* Zera a parte alta
	clrl	r2

	up			|* De baixo para cima 
2$:
	decl	r1
	jlt	4$

	lodsb
   |***	movb	(r4)+,r0
   	movb	(r5),r2
	incl	r5
   |***	movb	(r5)+,r2

	movb	(r3,r0),r0
	cmpb	(r3,r2),r0

	jhi	5$
	jlo	6$

	jmp	2$

|*	Terminou uma das tabelas sem decisão
4$:
	movl	24(fp),r3	|* "tba"
	clrl	24(fp)

	tstl	r3		|* Se não tiver tabela seguinte, ...
	jz	8$

	jmp	1$

5$:
	movl	#-1,r0		|* "dst" < "src"
	jmp	9$

6$:
	movl	#1,r0		|* "dst" > "src"
	jmp	9$

8$:
	clrl	r0		|* "dst" == "src"
9$:
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
