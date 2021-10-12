|*
|****************************************************************
|*								*
|*			umount.s				*
|*								*
|*	Desmonta um sistema de arquivos				*
|*								*
|*	Versão	3.0.0, de 16.01.95				*
|*		4.3.0, de 05.10.02				*
|*								*
|*	Módulo: umount						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

UMOUNT		= 22
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "umount" ******************************
|*
|*	int	umount (char *dev, int flags);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_umount
_umount:
	movl	#UMOUNT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
