|*
|****************************************************************
|*								*
|*			strncpy.s				*
|*								*
|*	Copia uma cadeia dado o comprimento			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		3.0.0, de 03.01.95				*
|*								*
|*	Módulo: strncpy						*
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
|*	Copia uma cadeia dado o comprimento			*
|****************************************************************
|*
|*	char	*strncpy (char *dst, const char *src, size_t count);
|*
	.global	_strncpy
_strncpy:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	up
1$:
	decl	r1		|* Analisa o comprimento
	jlt	9$

	movsb			|* Copia um byte
   |***	movb	(r4)+,(r5)+

	cmpb	#0,-1(r4)	|* Verifica se chegou no NULO
	jne	1$

	clrb	r0		|* Zera o resto da área
	rep
	stosb	
9$:
	movl	8(fp),r0	|* retorna "dst"

	popl	r4
	popl	r5
	unlk
	rts
