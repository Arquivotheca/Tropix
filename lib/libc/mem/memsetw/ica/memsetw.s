|*
|****************************************************************
|*								*
|*			memsetw.s				*
|*								*
|*	Preenche uma área com uma palavra (16 bits)		*
|*								*
|*	Versão	2.1.0, de 11.02.89				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Modulo: memsetw						*
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
|*	Preenche uma área com uma palavra (16 bits)		*
|****************************************************************
|*
|*	void *
|*	memsetw (void *area, int word, size_t n)
|*
|*	area    - Area de memoria 
|*	word    - Palavra (16 bits) a ser escrita
|*	n       - Número de palavras a escrever
|*
	.global	_memsetw

_memsetw:
	movl	4(sp),a0	|* area
	movw	2+8(sp),d0	|* palavra em d0
	movl	12(sp),d1	|* n
	jra	teste
loop:
	movw	d0,(a0)+
teste:
	dbra	d1,loop
	subl	#0x00010000,d1
	jhs	loop
sai:
	movl	4(sp),d0	|* retorna area
	rts
