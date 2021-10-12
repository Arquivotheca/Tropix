|*
|****************************************************************
|*								*
|*			strmove.s				*
|*								*
|*	Copia uma cadeia com superposição			*
|*								*
|*	Versão	1.0.0, de 06.02.87				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Módulo: strmove						*
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
|*	Copia uma cadeia com superposição			*
|****************************************************************
|*
|*	char *
|*	strmove (char *dst, const char *src)
|*
	.global	_strmove

_strmove:
	movl	4(sp),a1	|* Destino
	movl	8(sp),a0	|* Fonte
	movl	a1,d0

	cmpl	a1,a0
	jgt	paracima	|* desvia se a0 > a1
	jlt	parabaixo	|* desvia se a0 < a1
	rts
	
|*
|*	a0 > a1 => Copia para cima
|*
paracima:
1$:
	movb	(a0)+,(a1)+
	jne	1$

	rts

|*
|*	a0 < a1 => Copia para baixo
|*
parabaixo:
1$:
	tstb	(a0)+		|* Sai do loop com "a0" depois do NULO !
	jne	1$

	movl	a0,d0		|* "d0" contem "strlen (fonte) + 1"
	subl	8(sp),d0
	addl	d0,a1
	
	jra	teste
loop:
	movb	-(a0),-(a1)
teste:
	dbra	d0,loop
	subl	#0x00010000,d0
	jhs	loop

	movl	a1,d0
	rts
