|*
|****************************************************************
|*								*
|*			chown.s					*
|*								*
|*	Muda o dono e o grupo de um arquivo			*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 12.08.95				*
|*								*
|*	Módulo: chown						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

CHOWN		= 16
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "chown" ******************************
|*
|*	int	chown (const char *path, int owner, int group);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_chown
_chown:
	movl	#CHOWN,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
