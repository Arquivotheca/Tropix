|*
|****************************************************************
|*								*
|*			compute_checksum.c			*
|*								*
|*	Calcula o CHECKSUM					*
|*								*
|*	Versão	3.0.00, de 27.11.92				*
|*		3.0.00, de 27.11.92				*
|*								*
|*	Módulo: Internet					*
|*		NÚCLEO DO SISTEMA				*
|*		Categoria E					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|* 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Cálculo do Checksum					*
|****************************************************************
|*
	.text
	.global	_compute_checksum

|*
|*	checksum = compute_checksum (void *area, int count);
|*
|*	Supõe "area" par
|*
_compute_checksum:
	link	fp,#-4
	movl	d2,(sp)

	movl	8(fp),a0	|* area
	movl	12(fp),d1	|* count

|*
|*	Inicia o cálculo
|*
	moveq	#0,d0		|* Zera o resultado

	movl	d1,d2
	lsrl	#6,d1		|* count/64 = # loop traversals
	andl	#0x3C,d2	|* Then find fractions of a chunk
	negl	d2

	andb	#0x0F,cc	|* Clear X (extended carry flag)

	jmp	6$-.-2(pc,d2)	|* Desvia para a malha

|*
|*	Malha interna (processa de 32 em 32 bits)
|*
5$:
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
	movl	(a0)+,d2
	addxl   d2,d0
6$:
	dbra	d1,5$

|*
|*	Verifica se tem 1, 2 ou 3 bytes ainda
|*
|*	Repare que as instruções em geral não afetam o X
|*
	movl	12(fp),d1	|* count

	btst	#1,d1
	jeq	7$

	movw	(a0)+,d2
	addxw   d2,d0

7$:
	btst	#0,d1
	jeq	8$

	movw	(a0)+,d2
	clrb	d2
	addxw   d2,d0

|*
|*	Coda
|*
8$:
	movl	d0,d1		|* Fold 32 bit sum to 16 bits
	swap	d1		|* (NB- swap doesn't affect X)
	addxw   d1,d0
	jcc	9$
	addw	#1,d0
9$:
	andl	#0x0000FFFF,d0
	notw	d0

	movl	(sp),d2
	unlk	fp
	rts	
