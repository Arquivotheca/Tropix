|*
|****************************************************************
|*								*
|*			strpbrk.s				*
|*								*
|*	Procura caracteres em uma cadeia			*
|*								*
|*	Versão	1.0.0, de 16.10.86				*
|*		3.0.0, de 09.06.95				*
|*								*
|*	Módulo: strpbrk						*
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
|*	Procura caracteres em uma cadeia			*
|****************************************************************
|*
|*	char	*strpbrk (const char *str, const char *list)
|*
|*	str  - cadeia com os tokens 
|*	list - cadeia com a lista de caracteres procurados
|*
	.globl	_strpbrk
_strpbrk:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r4	|* "str"
   |***	movl	12(fp),r5	|* "list"
	up			|* Sempre para cima

|*
|****** Malha de busca ******************************************
|*
1$:
	lodsb			|* Pega o caractere seguinte de "str"
   |***	movb	(r4)+,r0
	tstb	r0
	jeq	not_found	|* fim de "str"

	movl	12(fp),r5	|* Ponteiro para a lista
2$:
	cmpb	#0,(r5)		|* Testa o final dos separadores
	jeq	1$
	scasb			|* Compara o caractere com o separador
   |***	cmpb	r0,(r5)+
	jne	2$		|* Se não for da lista, ...

|*
|****** Achou! **************************************************
|*
	movl	r4,r0
	decl	r0

	jmp	return

not_found:
	clrl	r0		|* "str" chegou ao fim
return:
	popl	r4
	popl	r5
	unlk
	rts
