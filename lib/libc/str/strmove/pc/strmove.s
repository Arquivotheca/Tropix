|*
|****************************************************************
|*								*
|*			strmove.s				*
|*								*
|*	Copia uma cadeia com superposição			*
|*								*
|*	Versão	1.0.0, de 06.02.87				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: strmove						*
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
|*	Copia uma cadeia com superposição			*
|****************************************************************
|*
|*	char *
|*	strmove (char *dst, const char *src)
|*
	.global	_strmove
_strmove:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"

	cmpl	r5,r4
	jhi	paracima	|* desvia se "src" > "dst"
	jlo	parabaixo	|* desvia se "src" < "dst"
	rts

paracima:
	up
1$:
	movsb			|* Copia um byte (e o NULO final)
   |*** movb	(r4)+,(r5)+

	cmpb	#0,-1(r4)
	jne	1$

	jmp	exit

parabaixo:
	movl	r4,r5		|* "src" no r5 !
	movl	#-1,r1		|* Maior valor possível para o contador
	clrb	r0		|* Valor = 0

	up			|* Procura o final da cadeia
	repne
	scasb
   |*** cmpb	(r5)+,r0

	movl	r5,r1		|* Calcula o comprimento
	subl	12(fp),r1

	movl	r5,r4		|* Obtém "src"
	decl	r4

	movl	8(fp),r5	|* Obtém "dst"
	addl	r1,r5
	decl	r5

	down			|* Copia a cadeia (com o NULO final)
	rep
	movsb
   |*** movb	(r4)-,(r5)-

exit:
	movl	8(fp),r0	|* Retorna "dst"
	popl	r4
	popl	r5
	unlk
	rts
