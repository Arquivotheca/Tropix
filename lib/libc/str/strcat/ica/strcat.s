|*
|****************************************************************
|*								*
|*			strcat.s				*
|*								*
|*	Concatena cadeias					*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 18.04.91				*
|*								*
|*	Módulo: strcat						*
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
|*	Concatena cadeias					*
|****************************************************************
|*
|*	char *
|*	strcat (char *dst, const char *src)
|*
	.global	_strcat

_strcat:
	movl	4(sp),a1	|* dst
	movl	8(sp),a0	|* src
	movl	a1,d0

loop1:
	tstb	(a1)+
	jne	loop1

	subql	#1,a1

loop2:
	movb	(a0)+,(a1)+
	jne	loop2

	rts
