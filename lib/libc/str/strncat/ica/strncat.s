|*
|****************************************************************
|*								*
|*			strncat.s				*
|*								*
|*	Concatena cadeias dado o comprimento			*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Módulo: strncat						*
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
|*	Concatena cadeias dado o comprimento			*
|****************************************************************
|*
|*	char *
|*	strncat (char *dst, const char *src, size_t nbytes)
|*
	.global	_strncat

_strncat:
	movl	4(sp),a1	|* dst
	movl	a1,d0
	movl	8(sp),a0	|* src

end_loop:
	tstb	(a1)+		|* Acha o fim da cadeia dst
	jne	end_loop

	subql	#1,a1		|* Volta para o '\0'

	movl	12(sp),d1	|* nbytes

	jra	teste 		|* Código de condição de nbytes

cat_loop:
	movb	(a0)+,(a1)+	|* move a cadeia src
teste:
	dbeq	d1,cat_loop
	jeq	fim
	subl	#0x00010000,d1
	jhs	cat_loop
fim:
	rts
