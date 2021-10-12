|*
|****************************************************************
|*								*
|*			seteuid.s				*
|*								*
|*	Atribui o EUID do processo				*
|*								*
|*	Versão	3.0.0, de 21.08.95				*
|*		3.0.0, de 21.08.95				*
|*								*
|*	Módulo: seteuid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETREUID	 = 73
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "seteuid" ****************************
|*
|*	int	seteuid (int euid);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_seteuid
_seteuid:
	movl	4(sp),r0	|* Salva o EUID
	pushl	r0		|* Prepara o EUID
	pushl	#-1		|* O RID == -1 (não muda)
	pushl	r0		|* Coloca um endereço de retorno fictício

	movl	#SETREUID,r0
	callgl	#SYS_CALL,0

	popl	r1		|* Retira o endereço de retorno fictício
	popl	r1		|* Retira o -1
	popl	r1		|* Retira o EUID

	tstl	r2
	jnz	syscall_error

	rts
