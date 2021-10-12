|*
|****************************************************************
|*								*
|*			memtcmp.s				*
|*								*
|*	Compara duas áreas de memória dada uma tabela		*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Modulo: memtcmp						*
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
|*	Compara duas áreas de memória dada uma tabela		*
|****************************************************************
|*
|*	int
|*	memtcmp (const void *area1, const void *area2,
|*					size_t nbytes, char table[])
|*
	.global	_memtcmp

_memtcmp:
	movl	4(sp),a0	|* area1
	movl	8(sp),a1	|* area2

	moveml	#<a2,d2>,-(sp)	|* Preciso de mais 2 registros

	moveq	#0,d0		|* Zera a parte alta
	moveq	#0,d1

	cmpl	a0,a1		|* Se for a mesma área, ... (d0 == 0)
	jeq	exit

	movl	8+12(sp),d2	|* nbytes
	movl	8+16(sp),a2	|* Endereço da tabela

	jra	teste
loop:
	movb	(a0)+,d0	|* Prepara os caracteres
	movb	(a1)+,d1

	movb	0(a2,d0),d0	|* indexa a tabela e compara
	cmpb	0(a2,d1),d0

	jlo	menor
	jhi	maior
teste:
	dbra	d2,loop
	subl	#0x00010000,d2
	jhs	loop

	moveml	(sp)+,#<a2,d2>
	jra	_memcmp		|* desempata pelo ASCII

menor:
	moveq	#-1,d0
	jra	exit
maior:
	moveq	#1,d0
exit:
	moveml	(sp)+,#<a2,d2>
	rts
