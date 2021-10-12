|*
|****************************************************************
|*								*
|*			kcntl.s					*
|*								*
|*	Controle do KERNEL					*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 20.12.94				*
|*								*
|*	Módulo: kcntl						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

KCNTL		= 106
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "kcntl" *******************************
|*
|*	int	kcntl (int cmd, int arg1, int arg2)
|* 
|*	Retorno: Sucesso: 0; Erro: -1
|* 
	.global	_kcntl
_kcntl:
	movl	#KCNTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
