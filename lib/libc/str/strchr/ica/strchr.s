|*
|****************************************************************
|*								*
|*			strchr.s				*
|*								*
|*	Indica a primeira ocorrência de um caractere		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strchr						*
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
|*	Indica a primeira ocorrência de um caractere		*
|****************************************************************
|*
|*	char *
|*	strchr (const char *cad, int carac)
|*
	.globl	_strchr

_strchr:
	movl	4(sp),a0	|* cad
	movb	3+8(sp),d0	|* carac em d0

loop:
	cmpb	(a0),d0
	jeq	achou		|* achou igual

	tstb	(a0)+
	jne	loop		|* não é fim de cad

	moveq	#0,d0
	rts

achou:
	movl	a0,d0		|* retorna onde achou
	rts

