|*
|****************************************************************
|*								*
|*			memcpy.s				*
|*								*
|*	Copia uma área de memoria				*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: memcpy						*
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
|*	Copia uma área de memoria				*
|****************************************************************
|*
|*	void	*memcpy (void *dst, const void *src, size_t count);
|*
	.global	_memcpy
_memcpy:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	movl	r5,r0		|* Retorna "dst"

	up			|* Cópia de baixo para cima 
	lsrl	#2,r1
	rep
	movsl

	movl	16(fp),r1	|* Copia os 3 bytes restantes
	andl	#3,r1
	rep
	movsb

	popl	r4
	popl	r5
	unlk
	rts
