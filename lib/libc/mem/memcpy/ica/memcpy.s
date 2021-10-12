|*
|****************************************************************
|*								*
|*			memcpy.s				*
|*								*
|*	Copia uma área de memoria				*
|*								*
|*	Versão	1.0.0, de 07.10.86				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Módulo: memcpy						*
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
|*	Copia uma área de memoria				*
|****************************************************************
|*
|*	void *
|*	memcpy (void *dst, const void *src, size_t nbytes)
|*
	.global	_memcpy

_memcpy:
	movl	4(sp),a1	|* dst
	movl	a1,d0		|* guarda retorno
	movl	8(sp),a0	|* src
	movl	12(sp),d1	|* nbytes
	jra	teste
loop:
	movb	(a0)+,(a1)+
teste:
	dbra	d1,loop
	subl	#0x00010000,d1
	jhs	loop
sai:
	rts
