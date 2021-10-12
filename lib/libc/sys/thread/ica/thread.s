|*
|****************************************************************
|*								*
|*			thread.s				*
|*								*
|*	Cria uma linha de execução (thread)			*
|*								*
|*	Versão	1.0.0, de 24.04.91				*
|*		2.3.0, de 24.04.91				*
|*								*
|*	Modulo: thread						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|* 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	int
|*	thread ()
|*
|*	Pai   = Thread criador;
|*	Filho = Thread criado;
|*
|*	Retorna ao Pai   = Sucesso: Pid do filho; Erro: -1
|*	Retorna no Filho = Sucesso: 0
|*
	.global	_thread, _errno
 
THREAD = 118
 
_thread:
	moveq	#THREAD,d0
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
