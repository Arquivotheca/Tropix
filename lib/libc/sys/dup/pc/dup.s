|*
|****************************************************************
|*								*
|*			dup.s					*
|*								*
|*	Duplica um descritor de arquivos			*
|*								*
|*	Versão	1.0.0, de 12.01.86				*
|*		3.0.0, de 13.08.95				*
|*								*
|*	Módulo: dup						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

DUP		= 41
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamadas ao sistema "dup" e "dup2" **********************
|*
|*	int	dup (int fd1);
|*
|*	int	dup2 (int fd1, int fd2);
|*
|*	Retornam = Sucesso: fd (>= 0); Erro: -1
|*
	.global	_dup, _dup2
_dup2:
	orl	#0x40,4(sp)
_dup:
	movl	#DUP,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
