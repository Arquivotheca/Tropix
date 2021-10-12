|*
|****************************************************************
|*								*
|*			strspn.s				*
|*								*
|*	Tamanho do segmento com os caracteres			*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strspn						*
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
|*	Tamanho do segmento com os caracteres			*
|****************************************************************
|*
|*	size_t
|*	strspn (const char *cad, const char *lista)
|*
|*	cad   - cadeia 
|*	lista - cadeia contendo a lista dos caracteres
|*
	.globl	_strspn

_strspn:
	movl	4(sp),a0	|* cad
	movl	8(sp),a1	|* lista
	movl	a1,d1		|* guarda lista em d1

loop1:
	movl	d1,a1		|* restaura a lista
	movb	(a0)+,d0	|* carac. de cad em d0
	jeq	dif		|* fim de cadeia

loop2:
	cmpb	(a1),d0
	jeq	loop1		|* achou igual

	tstb	(a1)+
	jne	loop2		|* não é fim da lista

dif:
	movl	a0,d0		|* ve quanto a0 andou
	subl	4(sp),d0
	subql	#1,d0
	rts

