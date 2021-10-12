|*
|****************************************************************
|*								*
|*			strncpy.s				*
|*								*
|*	Copia uma cadeia dado o comprimento			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Módulo: strncpy						*
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
|*	Copia uma cadeia dado o comprimento			*
|****************************************************************
|*
|*	char *
|*	strncpy (char *dst, const char *src, size_t nbytes)
|*
	.global	_strncpy

_strncpy:
	movl	4(sp),a1	|* dst
	movl	a1,d0
	movl	8(sp),a0	|* src
	movl	12(sp),d1	|* nbytes
	jra	cp_test

cp_loop:
	movb	(a0)+,(a1)+
	jeq	clr_test
cp_test:
	dbra	d1,cp_loop
	subl	#0x00010000,d1
	jhs	cp_loop

	rts
	
clr_loop:
	clrb	(a1)+
clr_test:
	dbra	d1,clr_loop
	subl	#0x00010000,d1
	jhs	clr_loop
fim:
	rts
