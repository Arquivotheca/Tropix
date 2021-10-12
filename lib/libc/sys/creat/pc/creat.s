|*
|****************************************************************
|*								*
|*			creat.s					*
|*								*
|*	Cria um arquivo novo ou escreve em um antigo		*
|*								*
|*	Versão	3.0.0, de 22.12.94				*
|*		3.0.0, de 22.12.94				*
|*								*
|*	Módulo: creat						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

CREAT		= 8
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "creat" *******************************
|*
|*	int	creat (const char *path, int mode);
|*
|*	Retorno = Sucesso: um fd >= 0; Erro: -1
|*
	.global	_creat
_creat:
	movl	#CREAT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
