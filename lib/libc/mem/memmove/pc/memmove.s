|*
|****************************************************************
|*								*
|*			memmove.s				*
|*								*
|*	Rotina otimizada de copia de bytes/longs		*
|*								*
|*	Versão	3.0.0, de 14.01.95				*
|*		3.0.0, de 07.07.95				*
|*								*
|*	Módulo: memmove						*
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
|*	Rotina otimizada de copia de bytes/longs		*
|****************************************************************
|*
|*	void	*memmove (void *dst, void *src, int count);
|*
	.global	_memmove
_memmove:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	cmpl	r4,r5		|* Verifica o sentido da cópia
	jhi	5$

	up			|* Cópia de baixo para cima 
	lsrl	#2,r1
	rep
	movsl

	movl	16(fp),r1	|* Copia os 3 bytes restantes
	andl	#3,r1
	rep
	movsb

	jmp	9$
5$:
	down			|* Cópia de cima para baixo
	movl	r1,r0
	decl	r0
	addl	r0,r4
	addl	r0,r5

	andl	#3,r1		|* Copia inicialmente 3 bytes
	rep
	movsb

	movl	16(fp),r1	|* Copia os longos restantes
	lsrl	#2,r1
	subl	#3,r4
	subl	#3,r5

	rep
	movsl
9$:
	movl	8(fp),r0	|* Retorna "dst"

	popl	r4
	popl	r5
	unlk
	rts
