|*
|****************************************************************
|*								*
|*			link.s					*
|*								*
|*	Cria uma entrada de diretório				*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 15.01.95				*
|*								*
|*	Módulo: link						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

LINK		= 9
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "link" *******************************
|*
|*	int	link (char *path1, char *path2);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_link
_link:
	movl	#LINK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
