|*
|****************************************************************
|*								*
|*			memchr.s				*
|*								*
|*	Indica a primeira ocorrência de um caractere		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.1, de 14.04.91				*
|*								*
|*	Módulo: memchr						*
|*		libc/mem					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Indica a primeira ocorrência de um caractere		*
|****************************************************************
|*
|*	void *
|*	memchr (void *cad, int carac, size_t nbytes)
|*
|*	cad    - cadeia 
|*	carac  - Caracter a ser procurado
|*	nbytes - tamanho da area
|*
	.global	_memchr

_memchr:
	movl	4(sp),a0	|* cad
	movb	3+8(sp),d0	|* carac em d0
	movl	12(sp),d1	|* nbytes
	jeq	não_achou
	jra	teste
loop:
	cmpb	(a0)+,d0
teste:
	dbeq	d1,loop		|* sai se achou igual
	jeq	achou
	subl	#0x00010000,d1
	jhs	loop
não_achou:
	moveq	#0,d0
	rts
achou:
	movl	a0,d0		|* retorna onde achou
	subql	#1,d0
	rts
