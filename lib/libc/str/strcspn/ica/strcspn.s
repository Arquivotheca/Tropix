|*
|****************************************************************
|*								*
|*			strcspn.s				*
|*								*
|*	Tamanho do segmento sem os caracteres			*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strcspn						*
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
|*	Tamanho do segmento sem os caracteres			*
|****************************************************************
|*
|*	size_t
|*	strcspn (const char *cad, const char *lista)
|*
|*	cad   - cadeia 
|*	lista - cadeia contendo a lista dos caracteres
|*
	.global	_strcspn

_strcspn:
	movl	4(sp),a0	|* cad
	movl	8(sp),a1	|* lista
	movl	a1,d1		|* guarda lista em d1

loop1:
	movb	(a0)+,d0	|* carac. de cad em d0
	jeq	igual		|* fim de cadeia

loop2:
	cmpb	(a1),d0
	jeq	igual		|* achou um igual

	tstb	(a1)+
	jne	loop2		|* não é fim da lista

	movl	d1,a1		|* restaura a lista
	jra	loop1

igual:
	movl	a0,d0		|* ve quanto a0 andou
	subl	4(sp),d0
	subql	#1,d0
	rts
