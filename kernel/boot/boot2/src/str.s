|*
|****************************************************************
|*								*
|*			str.s					*
|*								*
|*	Manipulação de cadeias de caracteres			*
|*								*
|*	Versão	3.0.0, de 08.09.94				*
|*		4.3.0, de 29.05.02				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*


|*
|****************************************************************
|*	Copia uma cadeia de caracteres				*
|****************************************************************
|*
|*	char *
|*	strcpy (char *dst, const char *src)
|*
	.global	_strcpy
_strcpy:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"

	movl	r5,r0		|* Retorna "dst"

	up
1$:
	cmpb	#0,(r4)		|* Testa o final de "src"
	jz	2$

	movsb			|* Copia "src"
   |***	movb	(r4)+,(r5)+
	jmp	1$
2$:
	movsb			|* Copia o NULO final

	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Compara cadeais de caracteres				*
|****************************************************************
|*
|*	int	streq (const char *str1, const char *str2);
|*
	.global	_streq
_streq:
	movl	4(sp),r1	|* str1
	movl	8(sp),r2	|* str2
2$:
	movb	(r1),r0
	cmpb	(r2),r0
	jne	9$

	tstb	r0
	jne	5$

	incl	r0		|* retorna 1
	rts
5$:
	incl	r1
	incl	r2

	jmp	2$
9$:
	clrl	r0		|* retorna 0
	rts

|*
|****************************************************************
|*	Obtém o tamanho de uma cadeia				*
|****************************************************************
|*
|*	int 	strlen (const char *src);
|*
	.global	_strlen
_strlen:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "src"
	movl	#-1,r1		|* Maior valor possível para o contador
	clrl	r0		|* Valor = 0

	up			|* Testa a cadeia
	repne
	scasb

	subl	8(fp),r5	|* Faz o cálculo do tamanho
	movl	r5,r0
	decl	r0

	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Obtém o final de uma cadeia				*
|****************************************************************
|*
|*	char *
|*	strend (const char *src)
|*
	.global	_strend
_strend:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "src"
	movl	#-1,r1		|* Maior valor possível para o contador
	clrl	r0		|* Valor = 0

	up			|* Testa a cadeia
	repne
	scasb

	movl	r5,r0		|* Passa um caractere
	decl	r0

	popl	r5
	unlk
	rts
