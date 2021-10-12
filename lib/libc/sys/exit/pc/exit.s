|*
|****************************************************************
|*								*
|*			exit.s					*
|*								*
|*	Termina a execução de um processo			*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.0.0, de 11.08.95				*
|*								*
|*	Módulo: exit						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

EXIT		= 1
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "exit" ******************************
|*
|*	void	exit (int code);
|*	void	_exit (int code);
|*
|*	Retorna = (não há retorno)
|*
	.global	_exit, __exit
_exit:
   	call	__cleanup
__exit:
	movl	#EXIT,r0
	callgl	#SYS_CALL,0
