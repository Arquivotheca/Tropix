|*
|****************************************************************
|*								*
|*			strscpy.s				*
|*								*
|*	Copia uma cadeia dado o comprimento			*
|*								*
|*	Versão	4.6.0, de 10.10.04				*
|*		4.6.0, de 10.10.04				*
|*								*
|*	Módulo: strscpy						*
|*		libc/str					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Copia uma cadeia dado o comprimento			*
|****************************************************************
|*
|*	char	*strscpy (char *dst, const char *src, size_t count);
|*
	.global	_strscpy
_strscpy:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"

	up
1$:
	decl	r1		|* Analisa o comprimento
	jge	3$

	movb	#0,-1(r5)	|* Garante o NULO final
	jmp	9$
3$:
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
