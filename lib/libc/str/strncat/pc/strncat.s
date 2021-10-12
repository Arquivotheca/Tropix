|*
|****************************************************************
|*								*
|*			strncat.s				*
|*								*
|*	Concatena uma cadeia dado o comprimento			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		3.0.0, de 24.02.95				*
|*								*
|*	Módulo: strncat						*
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
|*	char	*strncat (char *dst, const char *src, size_t count);
|*
	.global	_strncat
_strncat:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
   |***	movl	16(fp),r1	|* "count"

|*
|******	Obtem o final da cadeia "dst" ***************************
|*
	movl	#-1,r1		|* Maior valor possível para o contador
	clrb	r0		|* Valor = 0

	up			|* Testa a cadeia
	repne
	scasb
   |*** cmpb	(r5)+,r0

	decl	r5		|* Volta para o '\0'

|*
|****** Copia a cadeia "src" ************************************
|*
   	movl	16(fp),r1	|* "count"
1$:
	decl	r1		|* Analisa o comprimento
	jlt	9$

	movsb			|* Copia um byte
   |***	movb	(r4)+,(r5)+

	cmpb	#0,-1(r4)	|* Verifica se chegou no NULO
	jne	1$

   |***	clrb	r0		|* Zera o resto da área
   |***	rep
   |***	stosb	
9$:
	movl	8(fp),r0	|* retorna "dst"

	popl	r4
	popl	r5
	unlk
	rts
