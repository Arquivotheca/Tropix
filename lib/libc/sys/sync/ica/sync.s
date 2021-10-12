|*
|****************************************************************
|*								*
|*			sync.s					*
|*								*
|*	Atualiza o conteúdo do disco				*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: sync						*
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
|*	sync ()
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_sync, _errno
 
SYNC = 36

_sync:
	moveq	#SYNC,d0
	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
