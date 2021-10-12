|*
|****************************************************************
|*								*
|*			getrand.s				*
|*								*
|*	Rotina para gerar nos. aleatórios de 32 bits		*
|*								*
|*	Versão	1.0.0, de 15.09.86				*
|*		3.0.0, de 19.02.95				*
|*								*
|*	Modulo: getrand						*
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
|*	Função para inicializar os parametros do gerador	*
|****************************************************************
|*
|*	void	setrand (unsigned long mult, unsigned long incr, int iter); 
|*
	.text
	.global	_setrand
_setrand:
	movl	4(sp),r0
	movl	r0,mult

	movl	8(sp),r0
	movl	r0,incr

	movl	12(sp),r0
	movl	r0,iter

	rts

|*
|****************************************************************
|*	Função geradora de números aleatórios			*
|****************************************************************
|*
|*	unsigned long	getrand (unsigned long num);
|*
	.global	_getrand
_getrand:
	movl	iter,r1		|* Prepara o no. de iterações
	incl	r1

	movl	4(sp),r0	|* Prepara o argumento

	jmp	9$
1$:
	movl	mult,r2		|* Multiplica
	mulul	r2,r0

	addl	incr,r0		|* O incremento
9$:
	loop	1$		|* Executa o no. de iterações

	rts

|*
|****************************************************************
|*	Área de dados inicializada com os valores da IBM	*
|****************************************************************
|*
	.data
mult:
	.long	314159269
incr:
	.long	453806245
iter:
	.long	1
