|*
|****************************************************************
|*								*
|*			ioctl.s					*
|*								*
|*	Controla dispositivos					*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.0.0, de 19.12.94				*
|*								*
|*	Módulo: ioctl						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

IOCTL		= 54
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "ioctl" ******************************
|*
|*	int	ioctl (int fd, int cmd, ...);
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_ioctl
_ioctl:
	movl	#IOCTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
