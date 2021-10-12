|*
|****************************************************************
|*								*
|*			memeq.s					*
|*								*
|*	Compara duas áreas de memória				*
|*								*
|*	Versão	1.0.0, de 06.10.86				*
|*		2.3.1, de 13.04.91				*
|*								*
|*	Módulo: memeq						*
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
|*	Compara duas áreas de memória				*
|****************************************************************
|*
|*	int
|*	memeq (const void *area1, const void *area2, size_t nbytes)
|*
	.global	_memeq

_memeq:
	movl	4(sp),a0	|* area1
	movl	8(sp),a1	|* area2

	cmpl	a0,a1		|* Se for a mesma área, ...
	jeq	igual

	movl	12(sp),d1	|* nbytes
	jra	teste
loop:
	cmpmb	(a1)+,(a0)+
	jne	dif
teste:
	dbra	d1,loop
	subl	#0x00010000,d1
	jhs	loop
igual:
	moveq	#1,d0
	rts
dif:
	moveq	#0,d0
	rts
