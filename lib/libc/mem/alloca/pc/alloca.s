|*
|****************************************************************
|*								*
|*			alloca.s				*
|*								*
|*	Aloca uma área na pilha de execução			*
|*								*
|*	Versão	3.0.0, de 29.01.95				*
|*		3.0.0, de 29.01.95				*
|*								*
|*	Modulo: alloca						*
|*		libc/mem					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	void *
|*	alloca (size)
|*	size_t	size;
|* 
|*	Retorno: Sucesso: Endereço da área (!=0); Erro: 0
|* 
	.global	_alloca
_alloca:
	movl	(sp),r1		|* Guarda o endereço de retorno

	movl	4(sp),r0	|* r0 = Tamanho arredondado
	addl	#3,r0
	andb	#~3,r0

	subl	r0,sp 		|* sp = começo da área - 4
	addl	#4,sp		|* Função que chamou retira mais 4 bytes

	movl	sp,r0		|* r0 = começo da área
	addl	#4,r0

	cmpb	#0,(r0)		|* Força a alocação

	jmp	r1		|* Retorna
