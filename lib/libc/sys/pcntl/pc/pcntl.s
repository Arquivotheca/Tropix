|*
|****************************************************************
|*								*
|*			pcntl.s					*
|*								*
|*	Controle do processo					*
|*								*
|*	Versão	3.0.0, de 17.08.95				*
|*		3.0.0, de 17.08.95				*
|*								*
|*	Módulo: pcntl						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

PCNTL		= 120
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "pcntl" *******************************
|*
|*	int	pcntl (int cmd, int arg1, int arg2)
|* 
|*	Retorno: Sucesso: 0; Erro: -1
|* 
	.global	_pcntl
_pcntl:
	movl	#PCNTL,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
