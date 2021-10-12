|*
|****************************************************************
|*								*
|*			symlink.s				*
|*								*
|*	Cria um elo simbólico					*
|*								*
|*	Versão	4.2.0, de 10.04.02				*
|*		4.2.0, de 16.04.02				*
|*								*
|*	Módulo: symlink						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SYMLINK		= 82
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "symlink" ****************************
|*
|*	int	symlink (const char *old_path, const char *new_path);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_symlink
_symlink:
	movl	#SYMLINK,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
