|*
|****************************************************************
|*								*
|*			strchr.s				*
|*								*
|*	Indica a primeira ocorrência de um caractere		*
|*								*
|*	Versão	3.0.0, de 22.12.94				*
|*		3.0.6, de 23.03.97				*
|*								*
|*	Módulo: strchr						*
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
|*	Indica a primeira ocorrência de um caractere		*
|****************************************************************
|*
|*	char *
|*	strchr (const char *str, int carac)
|*
	.globl	_strchr
_strchr:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "str"
	movb	12(fp),r0	|* "carac"

	up

	tstb	r0		|* Verifica se "carac" é NULO
	jne	1$

|*
|*	Caso de "carac" NULO
|*
	movl	#-1,r1		|* Maior valor possível para o contador

	repne			|* Procura o final da cadeia
	scasb

	jmp	7$

|*
|*	Caso de "carac" NÃO NULO
|*
1$:
	cmpb	#0,(r5)
	jne	3$

	clrl	r0		|* Não achou
	jmp	9$
3$:
	scasb			|* Procura o caracter
	jne	1$
7$:
	movl	r5,r0		|* Achou
	decl	r0
9$:
	popl	r5
	unlk
	rts
