|*
|****************************************************************
|*								*
|*			mem.s					*
|*								*
|*	Funções de cópia e atribuição para a memória		*
|*								*
|*	Versão	3.0.0, de 03.07.94				*
|*		4.0.0, de 15.01.01				*
|*								*
|*	Módulo: Boot2						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1997 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Função de cópia de áreas de memória			*
|****************************************************************
|*
|*	memmove (void *dst, void *src, int count);
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
	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Zera uma entrada de uma área ou estrutura		*
|****************************************************************
|*
|*	void	memclr (void *dst, int count);
|*
	.global	_memclr
_memclr:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r1	|* "count"
	clrl	r0		|* Valor = 0

	up			|* Atribui inicialmente longos
	lsrl	#2,r1
	rep
	stosl

	movl	12(fp),r1	|* Atribui os 3 bytes restantes
	andl	#3,r1
	rep
	stosb

	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Função de atribuição de área (char)			*
|****************************************************************
|*
|*	_memset (short *dst, char c, int count);
|*
	.global	_memset
_memset:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r0	|* "c"
	movl	16(fp),r1	|* "count"

	up			|* Baixo para cima 
	rep
	stosb

	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Função de atribuição de área (short)			*
|****************************************************************
|*
|*	memsetw (short *dst, short w, int word_count);
|*
	.global	_memsetw
_memsetw:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r0	|* "w"
	movl	16(fp),r1	|* "count"

	up			|* Baixo para cima 
	rep
	stosw

	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Função de atribuição de área (long)			*
|****************************************************************
|*
|*	_memsetl (short *dst, long c, int long_count);
|*
	.global	_memsetl
_memsetl:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r0	|* "l"
	movl	16(fp),r1	|* "long_count"

	up			|* Baixo para cima 
	rep
	stosl

	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Compara áreas de memoria				*
|****************************************************************
|*
|*	int	memeq (const void *dst, const void *src, size_t count);
|*
	.global	_memeq
_memeq:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	clrl	r0		|* Por enquanto, não é igual

	up			|* Comparação de baixo para cima 
	lsrl	#2,r1
	repe
	cmpsl

	jne	9$

	movl	16(fp),r1	|* Compara os 3 bytes restantes
	andl	#3,r1
	repe
	cmpsb

	jne	9$
	incl	r0		|* É igual
9$:
	popl	r4
	popl	r5
	unlk
	rts
