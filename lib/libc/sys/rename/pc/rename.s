|*
|****************************************************************
|*								*
|*			rename.s				*
|*								*
|*	Troca o nome de um arquivo				*
|*								*
|*	Versão	4.2.0, de 11.04.02				*
|*		4.2.0, de 11.04.02				*
|*								*
|*	Módulo: rename						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

RENAME		= 85
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "rename" *****************************
|*
|*	int	rename (char *path1, char *path2);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_rename
_rename:
	movl	#RENAME,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
