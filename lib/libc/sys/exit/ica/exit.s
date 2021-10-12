|*
|****************************************************************
|*								*
|*			exit.s					*
|*								*
|*	Termina um programa com atualização dos "buffers"	*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		2.3.0, de 05.09.88				*
|*								*
|*	Modulo: exit						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	void
|*	exit (status)
|*	int status;
|*
|*	Retorno = Não há
|*

	.global	_exit, __cleanup

EXIT = 1

_exit:
	jsr	__cleanup

	moveq	#EXIT,d0
	movl	4(sp),a0
	trap	#0
