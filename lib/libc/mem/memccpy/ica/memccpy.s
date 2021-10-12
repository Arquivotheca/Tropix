|*
|****************************************************************
|*								*
|*			memccpy.s				*
|*								*
|*	Copia uma área até encontrar um caractere		*
|*								*
|*	Versão	1.0.0, de 17.10.86				*
|*		2.3.1, de 14.04.91				*
|*								*
|*	Módulo: memccpy						*
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
|*	Copia uma área até encontrar um caractere		*
|****************************************************************
|*
|*	void *
|*	memccpy (void *dst, void *src, int carac, size_t nbytes)
|*
|*	dst   - área destino 
|*	src   - área origem
|*	carac - caractere
|*
	.globl	_memccpy

_memccpy:
	movl	4(sp),a1	|* dst
	movl	8(sp),a0	|* src
	movb	3+12(sp),d0	|* carac em d0
	movl	16(sp),d1	|* nbytes
	jeq	não_achou
	jra	teste
loop:
	movb	(a0)+,(a1)
	cmpb	(a1)+,d0
teste:
	dbeq	d1,loop		|* sai se achou ou nbytes
	jeq	achou
	subl	#0x00010000,d1
	jhs	loop
não_achou:
	moveq	#0,d0		|* Não achou
	rts
achou:
	movl	a1,d0		|* retorna onde achou + 1
	rts
