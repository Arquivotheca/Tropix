|*
|****************************************************************
|*								*
|*			chroot.s				*
|*								*
|*	Muda o diretório raiz					*
|*								*
|*	Versão	3.0.0, de 01.01.95				*
|*		3.0.0, de 12.08.95				*
|*								*
|*	Módulo: chroot						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

CHDIR		= 61
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "chroot" *****************************
|*
|*	int	chroot (const char *path);
|*
|*	Retorna = Sucesso: 0, erro: -1
|*
	.global	_chroot
_chroot:
	movl	#CHDIR,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
