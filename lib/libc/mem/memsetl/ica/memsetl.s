|*
|****************************************************************
|*								*
|*			memsetl.s				*
|*								*
|*	Preenche uma área com uma palavra longa (32 bits)	*
|*								*
|*	Versão	2.1.0, de 11.02.89				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Modulo: memsetl						*
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
|*	Preenche uma área com uma palavra longa (32 bits)	*
|****************************************************************
|*
|*	void *
|*	memsetl (void *area, int long, size_t n)
|*
|*	area    - Area de memoria 
|*	long    - Palavra (32 bits) a ser escrita
|*	n       - Número de palavras longas a escrever
|*
	.global	_memsetl

_memsetl:
	movl	4(sp),a0	|* area
	movl	8(sp),d0	|* palavra longa em d0
	movl	12(sp),d1	|* n
	jra	teste
loop:
	movl	d0,(a0)+
teste:
	dbra	d1,loop
	subl	#0x00010000,d1
	jhs	loop
sai:
	movl	4(sp),d0	|* retorna area
	rts
