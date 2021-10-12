|*
|****************************************************************
|*								*
|*			strncmp.s				*
|*								*
|*	Copia uma cadeia dado o comprimento			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		3.0.0, de 05.01.95				*
|*								*
|*	Módulo: strncmp						*
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
|*	Compara duas cadeias, dado o comprimento		*
|****************************************************************
|*
|*	int	strncmp (const char *str1, const char *str2, size_t count);
|*
|*	devolve  -1, 0 ou 1  se "str1" é menor, igual ou maior "str2"
|*
	.global	_strncmp
_strncmp:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "str1"
	movl	12(fp),r4	|* "str2"
	movl	16(fp),r1	|* "count"
	clrl	r0		|* Retorna "r0"

	cmpl	r4,r5		|* são uma mesma cadeia
	jeq	igual

	up
1$:
	decl	r1
	jlt	igual

	cmpsb			|* Compara um par de caracteres
|*	cmpb	(r5)+,(r4)+

	jlo	menor
	jhi	maior

	cmpb	#0,-1(r5)	|* Testa o final da cadeia
	jnz	1$	

	jmp	igual
menor:
	incl	r0		|* Retorna "1"
	jmp	exit
maior:
	decl	r0		|* Retorna "-1"
exit:
igual:
	popl	r4
	popl	r5
	unlk
	rts
