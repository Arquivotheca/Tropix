|*
|****************************************************************
|*								*
|*			strrchr.s				*
|*								*
|*	Indica a última ocorrência de um caractere		*
|*								*
|*	Versão	3.0.0, de 22.12.94				*
|*		3.0.6, de 23.03.97				*
|*								*
|*	Módulo: strrchr						*
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
|*	Indica a última ocorrência de um caractere		*
|****************************************************************
|*
|*	char	*strrchr (const char *str, int carac);
|*
	.globl	_strrchr
_strrchr:
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

	movl	r5,r0
	decl	r0
	jmp	9$

|*
|*	Caso de "carac" NÃO NULO
|*
1$:
	clrl	r1		|* Retorna "r1"
2$:
	cmpb	#0,(r5)
	jeq	7$

	scasb			|* Procura o caracter	
|*	cmpb	r0,(r5)+
	jne	2$

	movl	r5,r1		|* Achou o caracter
	decl	r1

	jmp	2$
7$:
	movl	r1,r0
9$:
	popl	r5
	unlk
	rts
