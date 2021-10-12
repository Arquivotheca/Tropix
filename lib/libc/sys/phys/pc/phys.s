|*
|****************************************************************
|*								*
|*			phys.s					*
|*								*
|*	Acessa diretamente a memória física			*
|*								*
|*	Versão	3.0.0, de 07.02.95				*
|*		3.0.0, de 07.02.95				*
|*								*
|*	Módulo: phys						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

PHYS		= 96
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "phys" *******************************
|*
|*	void	*phys (void *area, int count, int oflag);
|*
|*	Retorna = Sucesso: Endereco ou 0; Erro: -1
|*
	.global	_phys
_phys:
	movl	#PHYS,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
