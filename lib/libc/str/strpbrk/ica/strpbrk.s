|*
|****************************************************************
|*								*
|*			strpbrk.s				*
|*								*
|*	Procura caracteres em uma cadeia			*
|*								*
|*	Versão	1.0.0, de 16.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strpbrk						*
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
|*	Procura caracteres em uma cadeia			*
|****************************************************************
|*
|*	char *
|*	strpbrk (const char *cad, const char *lista)
|*
|*	cad   - cadeia 
|*	lista - cadeia contendo a lista dos caracteres
|*
	.global	_strpbrk

_strpbrk:
	movl	4(sp),a0	|* cad
	movl	8(sp),a1	|* lista
	movl	a1,d1		|* guarda lista em d1

loop1:
	movb	(a0)+,d0	|* carac. de cad em d0
	jeq	nachou		|* fim de cadeia
loop2:
	cmpb	(a1),d0
	jeq	achou		|* achou um igual
	tstb	(a1)+
	jne	loop2		|* não é fim da lista

	movl	d1,a1		|* restaura a lista
	jra	loop1

achou:
	subql	#1,a0
	movl	a0,d0		|* retorna apontando o achado
	rts
nachou:
	moveq	#0,d0		|* retorna nulo
	rts
