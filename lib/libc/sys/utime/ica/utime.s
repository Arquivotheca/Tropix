|*
|****************************************************************
|*								*
|*			utime.s					*
|*								*
|*	Modifica os tempos de um arquivo			*
|*								*
|*	Versão	1.0.0, de 30.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: utime						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	#include <sys/types.h>
|*
|*	int
|*	utime (path, timep)
|*	char	*path;
|*	time_t  *timep;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_utime, _errno
 
UTIME = 30

_utime:
	moveq	#UTIME,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
