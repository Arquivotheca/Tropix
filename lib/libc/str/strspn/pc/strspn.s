|*
|****************************************************************
|*								*
|*			strspn.s				*
|*								*
|*	Tamanho do segmento com os caracteres			*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		3.0.0, de 11.08.95				*
|*								*
|*	Módulo: strspn						*
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
|*	Tamanho do segmento com os caracteres			*
|****************************************************************
|*
|*	size_t	strspn (const char *str, const char *sep)
|*
|*	str - cadeia a ser examinada
|*	sep - cadeia com os separadores
|*
	.globl	_strspn
_strspn:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r4	|* "str"
   |***	movl	12(fp),r5	|* "sep"
	clrl	r1		|* Contador (resultado)
	up			|* Sempre para cima
1$:
	lodsb			|* Pega um caractere de "str"
   |***	movb	(r4)+,r0
	tstb	r0
	jz	9$		|* fim de "str"

	movl	12(fp),r5	|* Ponteiro para a cadeia de separadores
2$:
	cmpb	#0,(r5)		|* Testa o final dos separadores
	jeq	9$

	scasb			|* Compara o caractere com o separador
   |***	cmpb	r0,(r5)+
	jne	2$		|* Se não for separador, ...

	incl	r1		|* Encontrou um caractere que pertence
	jmp	1$
9$:
	movl	r1,r0		|* Resultado em "r0"

	popl	r4
	popl	r5
	unlk
	rts
