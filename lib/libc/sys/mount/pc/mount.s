|*
|****************************************************************
|*								*
|*			mount.s					*
|*								*
|*	Monta um sistema de arquivos				*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 15.01.95				*
|*								*
|*	Módulo: mount						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

MOUNT		= 21
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "mount" ******************************
|*
|*	int	mount (char *dev, char *dir, int flags);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mount
_mount:
	movl	#MOUNT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
