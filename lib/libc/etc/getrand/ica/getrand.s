|*
|****************************************************************
|*								*
|*			getrand.s				*
|*								*
|*	Rotina para gerar nos. aleatórios de 32 bits		*
|*								*
|*	Versão	1.0.0, de 15.09.86				*
|*		3.0.0, de 20.02.95				*
|*								*
|*	Módulo: getrand						*
|*		libc/etc					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Rotina que inicializa os parametros do gerador		*
|****************************************************************
|*
	.text
	.global	_setrand

_setrand:
	movl	4(sp),mult
	movl	8(sp),incr
	movl	12(sp),iter
	rts

|*
|****************************************************************
|*	Rotina que gera os números aleatórios			*
|****************************************************************
|*
	.global	_getrand

_getrand:
	movl	d2,-4(sp)
	movl	iter,d2		|* se iter == 0
	jne	teste

	movl	-4(sp),d2	|* restaura d2 e
	movl	4(sp),d0
	rts			|* retorna o mesmo argumento

repete:
	movw	6(sp),d0
	mulu	mult,d0		|* mult. partes baixas

	movw	4(sp),d1
	mulu	mult+2,d1	|* partes alta e baixa
	addl	d1,d0

	lsll	#8,d0
	lsll	#8,d0

	movw	6(sp),d1
	mulu	mult+2,d1	|* partes baixa e alta
	addl	d1,d0

	addl	incr,d0

	movl	d0,4(sp)

teste:
	dbra	d2,repete

	movl	-4(sp),d2	|* restaura d2 e

	rts			|* retorna

|*
|****************************************************************
|*	Area de dados inicializada com os valores da IBM	*
|****************************************************************
|*
	.data

mult:	.long	314159269
incr:	.long	453806245
iter:	.long	1
