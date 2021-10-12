|*
|****************************************************************
|*								*
|*			strcpy.s				*
|*								*
|*	Copia uma cadeia					*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.0, de 18.04.91				*
|*								*
|*	Módulo: strcpy						*
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
|*	Copia uma cadeia					*
|****************************************************************
|*
|*	char *
|*	strcpy (char *dst, const char *src)
|*
	.global	_strcpy

_strcpy:
	movl	4(sp),a1	|* dst
	movl	8(sp),a0	|* src
	movl	a1,d0
loop:
	movb	(a0)+,(a1)+
	jne	loop

	rts
