|*
|****************************************************************
|*								*
|*			alloca.s				*
|*								*
|*	Aloca uma área na pilha de execução			*
|*								*
|*	Versão	2.3.0, de 26.10.89				*
|*		2.3.1, de 26.12.90				*
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
	movl	(sp),a0		|* Guarda o endereço de retorno

	movl	4(sp),d0	|* d0 = Tamanho arredondado
	addql	#3,d0
	andw	#~3,d0

	subl	d0,sp 		|* sp = começo da área - 4
	addql	#4,sp		|* Função que chamou retira mais 4 bytes

	movl	sp,d0		|* d0 = começo da área
	addql	#4,d0

	movl	d0,a1		|* Força a alocação
	tstw	(a1)

	jmp	(a0)		|* Retorna
