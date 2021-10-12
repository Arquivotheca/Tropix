|*
|****************************************************************
|*								*
|*			str.s					*
|*								*
|*	Manipulação de cadeias de caracteres			*
|*								*
|*	Versão	3.0.0, de 08.09.94				*
|*		4.8.0, de 01.10.05				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*


|*
|****************************************************************
|*	Copia uma cadeia de caracteres				*
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

	popl	r4
	popl	r5
	unlk
	rts

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

|*
|****************************************************************
|*	Compara cadeais de caracteres				*
|****************************************************************
|*
|*	int	streq (const char *str1, const char *str2);
|*
	.global	_streq
_streq:
	movl	4(sp),r1	|* str1
	movl	8(sp),r2	|* str2
2$:
	movb	(r1),r0
	cmpb	(r2),r0
	jne	9$

	tstb	r0
	jne	5$

	incl	r0		|* retorna 1
	rts
5$:
	incl	r1
	incl	r2

	jmp	2$
9$:
	clrl	r0		|* retorna 0
	rts

|*
|****************************************************************
|*	Compara cadeias de caracteres sem considerar as caixas	*
|****************************************************************
|*
|*	int	strnocaseeq (const char *str1, const char *str2);
|*
	.global	_strnocaseeq
_strnocaseeq:
	link	#0
	pushl	r5
	pushl	r4
	pushl	r3

	movl	8(fp),r5		|* "str1"
	movl	12(fp),r4		|* "str2"
	movl	#_uppertolower,r3	|* "tb"

	cmpl	r4,r5		|* Se for a mesmas cadeia, ...
	jeq	7$

	clrl	r0		|* Zera a parte alta
	clrl	r2

	up			|* De baixo para cima 
2$:
	lodsb			|* Obtém os caracteres
   |***	movb	(r4)+,r0
   	movb	(r5),r2
	incl	r5
   |***	movb	(r5)+,r2

	movb	(r3,r0),r0	|* Compara um caractere
	cmpb	(r3,r2),r0
	jne	8$

	tstb	r2
	jne	2$
7$:
	clrl	r0		|* "str2" == "str2" =>	retorna 1
	incl	r0
	jmp	9$

8$:
	clrl	r0		|* "str2" != "str2" =>	retorna 0
9$:
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Obtém o tamanho de uma cadeia				*
|****************************************************************
|*
|*	int 	strlen (const char *src);
|*
	.global	_strlen
_strlen:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "src"
	movl	#-1,r1		|* Maior valor possível para o contador
	clrl	r0		|* Valor = 0

	up			|* Testa a cadeia
	repne
	scasb

	subl	8(fp),r5	|* Faz o cálculo do tamanho
	movl	r5,r0
	decl	r0

	popl	r5
	unlk
	rts

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

|*
|****************************************************************
|*	Converte UNICODE para ISO				*
|****************************************************************
|*
|*	char	*big_unicode2iso (char *dst, const char *src, int len);
|*	char	*little_unicode2iso (char *dst, const char *src, int len);
|*
	.global	_big_unicode2iso, _little_unicode2iso
_big_unicode2iso:
	incl	8(sp)		|* src: Avança para a parte baixa do unicode
_little_unicode2iso:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* dst
	movl	12(fp),r4	|* src
	movl	16(fp),r1	|* len

	up
2$:
	lodsb			|* Pega o caractere
   |***	movb	(r4)+,r0

	stosb			|* Armazena o caractere
   |***	movb	r0,(r5)+

	incl	r4 		|* Pula o '\0'

	decl	r1		|* Verifica se chegou ao final
	jgt	2$

	clrb	(r5)		|* Fim da cadeia

	movl	8(fp),r0	|* Retorna o endereço do destino

	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Calcula um número hash para uma cadeia de caracteres	*
|****************************************************************
|*
|*	int
|*	strhash (const char *name, in len, int size)
|*
	.global	_strhash
_strhash:
	movl	4(sp),r2	|* Endereço de name
	movl	8(sp),r1	|* Comprimento de name

	clrl	r0
1$:
	cmpl	#4,r1		|* Malha principal
	jlt	3$

	eorl	0(r2),r0

	addl	#4,r2
	subl	#4,r1
	jmp	1$
3$:
	movl	0(r2),r2	|* Pega os últimos 3 caracteres
	lsll	#2,r1
	andl	mask_vec(r1),r2
	eorl	r2,r0
11$:
	clrl	r2		|* Calcula a divisão
	divul	12(sp),r0
	movl	r2,r0

	rts

	.const
mask_vec:
	.long	0x00000000	|* Máscaras para extrair 1, 2, 3 bytes
	.long	0x000000FF
	.long	0x0000FFFF
	.long	0x00FFFFFF
