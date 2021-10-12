|*
|****************************************************************
|*								*
|*			select.s				*
|*								*
|*	Multiplexação de entrada síncrona			*
|*								*
|*	Versão	4.2.0, de 06.10.01				*
|*		4.2.0, de 06.10.01				*
|*								*
|*	Módulo: select						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SELECT	= 81
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "select" *****************************
|*
|*	int	select (int nfd, int *read_fd, int *write_fd, int *excep_fd, MUTMS *timeout)
|*
|*	Retorna = >= 0: Número de descritores prontos; Erro: -1
|*
	.global	_select
_select:
	movl	#SELECT,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
