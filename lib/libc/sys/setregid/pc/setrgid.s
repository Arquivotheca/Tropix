|*
|****************************************************************
|*								*
|*			setrgid.s				*
|*								*
|*	Atribui o RGID do processo				*
|*								*
|*	Versão	3.0.0, de 20.08.95				*
|*		3.0.0, de 20.08.95				*
|*								*
|*	Módulo: setrgid						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETREGID	 = 74
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "setrgid" ****************************
|*
|*	int	setrgid (int rgid);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_setrgid
_setrgid:
	movl	4(sp),r0	|* Salva o RGID
	pushl	#-1		|* O EID == -1 (não muda)
	pushl	r0		|* Prepara o RGID
	pushl	r0		|* Coloca um endereço de retorno fictício

	movl	#SETREGID,r0
	callgl	#SYS_CALL,0

	popl	r1		|* Retira o endereço de retorno fictício
	popl	r1		|* Retira o RGID
	popl	r1		|* Retira o -1

	tstl	r2
	jnz	syscall_error

	rts
