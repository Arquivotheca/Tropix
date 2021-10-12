|*
|****************************************************************
|*								*
|*			mknod.s					*
|*								*
|*	Cria um arquivo qualquer				*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 15.01.95				*
|*								*
|*	Módulo: mknod						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

MKNOD		= 14
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "mknod" ******************************
|*
|*	int	mknod (char *path, int mode, int dev)
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mknod
_mknod:
	movl	#MKNOD,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
