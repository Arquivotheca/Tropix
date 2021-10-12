|*
|****************************************************************
|*								*
|*			utime.s					*
|*								*
|*	Modifica os tempos de um arquivo			*
|*								*
|*	Versão	3.0.0, de 15.01.95				*
|*		3.0.0, de 15.01.95				*
|*								*
|*	Módulo: utime						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

UTIME		= 30
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "utime" ******************************
|*
|*	#include <sys/types.h>
|*
|*	int	utime (char *path, time_t timev[]);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_utime
_utime:
	movl	#UTIME,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
