|*
|****************************************************************
|*								*
|*			strrchr.s				*
|*								*
|*	Obtém a última ocorrência de um caractere		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.0, de 05.12.88				*
|*								*
|*	Módulo: strrchr						*
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
|*	Obtém a última ocorrência de um caractere		*
|****************************************************************
|*
|*	char *
|*	strrchr (const char *cad, int carac)
|*
|*	cad   - cadeia 
|*	carac - caractere a ser pesquisado
|*
	.global	_strrchr

_strrchr:
	movl	4(sp),a0	|* cad
	movb	3+8(sp),d1	|* carac em d1

	moveq	#0,d0		|* prepara retorno

loop:
	cmpb	(a0),d1
	jeq	achou		|* achou igual

	tstb	(a0)+
	jne	loop		|* não é fim de cad

	rts

achou:
	movl	a0,d0		|* guarda onde achou

	tstb	(a0)+		|* se não era nulo continua
	jne	loop

	rts
