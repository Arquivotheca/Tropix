|*
|****************************************************************
|*								*
|*			strhash.s				*
|*								*
|*	Calcula um número hash para uma cadeia de caracteres	*
|*								*
|*	Versão	4.0.0, de 13.08.01				*
|*		4.0.0, de 13.08.01				*
|*								*
|*	Módulo: strhash						*
|*		libc/str					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Calcula um número hash para uma cadeia de caracteres	*
|****************************************************************
|*
|*	int
|*	strhash (const char *name, in len, int size)
|*
	.global	_strhash
_strhash:
	movl	4(sp),r2	|* Endereço de name
	movl	8(sp),r1	|* Comprimento de name

	clrl	r0
1$:
	cmpl	#4,r1		|* Malha principal
	jlt	3$

	eorl	0(r2),r0

	addl	#4,r2
	subl	#4,r1
	jmp	1$
3$:
	movl	0(r2),r2	|* Pega os últimos 3 caracteres
	lsll	#2,r1
	andl	mask_vec(r1),r2
	eorl	r2,r0
11$:
	clrl	r2		|* Calcula a divisão
	divul	12(sp),r0
	movl	r2,r0

	rts

	.const
mask_vec:
	.long	0x00000000	|* Máscaras para extrair 1, 2, 3 bytes
	.long	0x000000FF
	.long	0x0000FFFF
	.long	0x00FFFFFF
