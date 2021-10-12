|*
|****************************************************************
|*								*
|*			strcat.s				*
|*								*
|*	Concatena cadeias					*
|*								*
|*	Versão	3.0.0, de 22.12.94				*
|*		3.0.0, de 07.08.95				*
|*								*
|*	Módulo: strcat						*
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
|*	Concatena cadeias					*
|****************************************************************
|*
|*	char *
|*	strcat (char *dst, const char *src)
|*
	.global	_strcat
_strcat:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	#-1,r1		|* Maior valor possível para o contador
	clrb	r0		|* Valor = 0

	up			|* Procura o final da cadeia
	repne
	scasb

	decl	r5		|* Aponta para o '\0'
1$:
	cmpb	#0,(r4)		|* Testa o final de "src"
	jz	2$

	movsb			|* Copia "src"
   |***	movb	(r4)+,(r5)+
	jmp	1$
2$:
	movsb			|* Copia o NULO final
   |***	movb	(r4)+,(r5)+

	movl	8(fp),r0	|* Retorna "dst"

	popl	r4
	popl	r5
	unlk
	rts
