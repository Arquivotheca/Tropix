|*
|****************************************************************
|*								*
|*			strcpy.s				*
|*								*
|*	Copia uma cadeia					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 06.01.95				*
|*								*
|*	Módulo: strcpy						*
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
|*	Copia uma cadeia					*
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
   |***	movb	(r4)+,(r5)+

	popl	r4
	popl	r5
	unlk
	rts
