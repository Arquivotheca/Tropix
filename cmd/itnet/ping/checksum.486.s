|*
|****************************************************************
|*								*
|*			checksum.s				*
|*								*
|*	Calcula o CHECKSUM					*
|*								*
|*	Versão	3.0.00, de 07.05.95				*
|*		3.0.00, de 07.05.95				*
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

CARRY	= -4		|* Deslocamento da variável

|*
|****************************************************************
|*	Cálculo do Checksum					*
|****************************************************************
|*
|*	checksum = compute_checksum (void *area, int count);
|*
	.text
	.global	_compute_checksum
_compute_checksum:
	link	#4
	pushl	r4

	movl	8(fp),r4	|* "area"
	movl	12(fp),r1	|* "count"

	up			|* Incrementa os ponteiros

	clrl	r2		|* Zera a soma
	movl	r2,CARRY(fp)	|* Zera o "carry"

	lsrl	#2,r1		|* Em longos
	testl	r1,r1		|* Zera o CF
	jz	2$

|*
|*	Malha principal, de 4 em 4 bytes
|*
1$:
	lodsl
   |***	movl	(r4)+,r0	
	adcl	r0,r2

	decl	r1		|* Não altera o CF!
	jgt	1$

|*
|*	Processa os 3 bytes restantes
|*
2$:
	setc	CARRY(fp)

	movl	12(fp),r1	|* "count"
	testb	#2,r1
	jz	3$

	lodsw			|* Processa 2 bytes seguintes
   |***	movw	(r4)+,r0	
	addw	r0,r2
	adcw	CARRY(fp),r2
	setc	CARRY(fp)
3$:
	testb	#1,r1
	jz	4$

	lodsw			|* Processa o byte final
   |***	movw	(r4)+,r0	
	clrb	h0
	addw	r0,r2
	adcw	CARRY(fp),r2
	setc	CARRY(fp)

|*
|*	Dobra de 32 para 16 bits
|*
4$:
	movl	r2,r0
	rorl	#16,r2		|* O "ror" altera o CF
	addw	r2,r0
	adcw	CARRY(fp),r0
	adcw	#0,r0

	andl	#0xFFFF,r0
	notw	r0

	popl	r4
	unlk
	rts
