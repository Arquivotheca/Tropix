|*
|****************************************************************
|*								*
|*			time.s					*
|*								*
|*	Obtem o tempo do sistema				*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: time						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

TIME		= 13
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "time" *******************************
|*
|*	time_t	time (time_t *tloc);
|*
|*	Retorna = Sucesso: Tempo (> 0); Erro: -1
|*
	.global	_time
_time:
	movl	#TIME,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	movl	4(sp),r1
	tstl	r1
	jz	1$

	movl	r0,(r1)
1$:
	rts
