|*
|****************************************************************
|*								*
|*			fork.s					*
|*								*
|*	Cria um processo					*
|*								*
|*	Versão	1.0.0, de 15.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: fork						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	int
|*	fork ()
|*
|*	Retorna no Pai   = Sucesso: Pid do filho; Erro: -1
|*	Retorna no Filho = Sucesso: 0
|*
	.global	_fork, _errno
 
FORK = 2
 
_fork:
	moveq	#FORK,d0
	trap	#0

	bras	1$		|* Retorno do Filho
	jcs	2$		|* Retorno do Pai
	rts
1$:
	rts			|* Filho
2$:
	movl	d0,_errno	|* Pai com Erro
	moveq	#-1,d0
	rts 
