|*
|****************************************************************
|*								*
|*			sematestl.s				*
|*								*
|*	Controle de semáforos do usuário			*
|*								*
|*	Versão	3.0.0, de 13.08.95				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: sematestl					*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SEMATESTL	= 116
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "sematestl" **************************
|*
|*	#include <sys/syscall.h>
|*	#include <unistd.h>
|*
|*	int	sematestl (int semadescr);
|*
|*	Retorna = Sucesso: >= 0; Erro: -1
|*
	.global	_sematestl
_sematestl:
	movl	#SEMATESTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
