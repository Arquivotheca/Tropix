|*
|****************************************************************
|*								*
|*			strncmp.s				*
|*								*
|*	Comparação de cadeias dado o comprimento		*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 14.04.91				*
|*								*
|*	Módulo: strncmp						*
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
|*	Comparação de cadeias dado o comprimento		*
|****************************************************************
|*
|*	int
|*	strncmp (char *src, const char *dst, nbytes)
|*
	.global	_strncmp

_strncmp:
	movl	4(sp),a0
	movl	8(sp),a1

	cmpl	a0,a1		|* São a mesma cadeia?
	jeq	igual

	movl	12(sp),d1	|* nbytes
	jra	teste
loop:
	movb	(a0)+,d0
	cmpb	(a1)+,d0
	jlo	menor
	jhi	maior

	tstb	d0
teste:
	dbeq	d1,loop
	jeq	igual
	subl	#0x00010000,d1
	jhs	loop
igual:
	moveq	#0,d0
	rts
menor:
	moveq	#-1,d0
	rts
maior:
	moveq	#1,d0
	rts
