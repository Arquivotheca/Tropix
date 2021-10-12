|*
|****************************************************************
|*								*
|*			strstr.s				*
|*								*
|*	Procura uma subcadeia em uma cadeia			*
|*								*
|*	Versão	2.3.0, de 07.03.90				*
|*		3.0.0, de 27.02.95				*
|*								*
|*	Módulo: strstr						*
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
|*	Procura uma subcadeia em uma cadeia			*
|****************************************************************
|*
|*	char *strstr (const char *str, const char *substr)
|*
	.global	_strstr
_strstr:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r4	|* "str"
   	movl	12(fp),r5	|* "substr"
	up			|* Sempre para cima
|*
|******	Verifica se a subcadeia é vazia *************************
|*
	movl	r4,r2		|* "r2" => Início de "str" do momento

	cmpb	#0,(r5)
	jeq	8$
|*
|******	Procura o primeiro caracter da subcadeia ****************
|*
1$:
	movl	r2,r4		|* Reinicializa a "str"
   	movl	12(fp),r5 	|* Reinicializa a "substr"

	movb	(r5),r1		|* Primeiro caractere de "substr"
2$:
	lodsb			|* Pega um caractere de "str"
   |***	movb	(r4)+,r0
	tstb	r0
	jz	6$

	cmpb	r1,r0
	jne	2$
|*
|******	O primeiro caracter confere, verifica o resto ***********
|*
	movl	r4,r2		|* Guarda o início da cadeia
	decl	r2

	incl	r5		|* Incrementa "substr"
4$:
	cmpb	#0,(r5)
	jeq	8$

	cmpsb
   |***	cmpb	(r4)+,(r5)+
	jeq	4$

	incl	r2		|* Prepara nova tentativa
	jmp	1$
|*
|****** Encontrou ***********************************************
|*
6$:
	clrl	r2		|* Não encontrou
8$:
	movl	r2,r0		|* Põe o resultado em "r0"

	popl	r4
	popl	r5
	unlk
	rts
