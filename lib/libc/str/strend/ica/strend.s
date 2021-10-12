|*
|****************************************************************
|*								*
|*			strend.s				*
|*								*
|*	Obtém a posição do nulo final de uma cadeia		*
|*								*
|*	Versão	1.0.0, de 20.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strend						*
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
|*	Obtém a posição do nulo final de uma cadeia		*
|****************************************************************
|*
|*	char *
|*	strend (const char *cad)
|*
	.globl	_strend

_strend:
	movl	4(sp),a0	|* cad

	moveq	#0,d0		|* nulo em d0
loop:
	cmpb	(a0)+,d0
	jne	loop		|* procurando

	subql	#1,a0		|* achou
	movl	a0,d0
	rts
