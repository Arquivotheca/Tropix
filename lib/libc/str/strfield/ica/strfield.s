|*
|****************************************************************
|*								*
|*			strfield.s				*
|*								*
|*	Extração de campos em uma cadeia			*
|*								*
|*	Versão	1.0.0, de 09.03.87				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strfield					*
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
|*	Extração de campos em uma cadeia			*
|****************************************************************
|*
|*	char *
|*	strfield (char *cad, int sep)
|*
|*	cad - cadeia com os tokens 
|*	sep - separador de campos
|*
	.globl	_strfield

_strfield:
	movl	8(sp),d1	|* d1 = separador
	movl	4(sp),d0	|* a0 = cadeia
	movl	d0,a0
	jne	loop1

	movl	prox,a0		|* resto de cad
	cmpl	#0,a0
	jeq	fim

	movl	a0,4(sp)	|* guarda o inicio do campo

loop1:
	movb	(a0)+,d0	|* desvia se achou o final da cadeia
	jeq	fim

	cmpb	d1,d0		|* desvia se não achou o separador
	jne	loop1

|*
|*	Achou o separador
|*
	movb	#0,-(a0)	|* poe o nulo
	addql	#1,a0
	movl	a0,prox		|* prepara proxima chamada

	movl	4(sp),d0	|* retorna o ender. do campo
	rts

|*
|*	Chegou no final da cadeia
|*
fim:
	clrl	prox		|* a cadeia chegou ao fim

	movl	4(sp),d0	|* retorna o ender. do campo
	rts

|*
|*	Areas e dados
|*
	.data

prox:	.long	0		|* comeco do campo seguinte
