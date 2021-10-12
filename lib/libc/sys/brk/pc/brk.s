|*
|****************************************************************
|*								*
|*			brk.s					*
|*								*
|*	Muda o tamanho do segmento de dados do processo		*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.2.3, de 25.11.99				*
|*								*
|*	Módulo: brk						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

BRK		= 17
SYS_CALL	= { 6 * 8 + 3 }

|*
|****************************************************************
|*	Muda o tamanho do segmento de dados do processo		*
|****************************************************************
|*
|*	int	brk (void *addr);
|*	retorno => sucesso: 0, erro: -1
|*
|*	void	*sbrk (int incr);
|*	retorno => sucesso: "break" antigo, erro: -1 
|*
	.global	_sbrk, _brk
 
|*
|******	Alteração diferencial ***********************************
|*
_sbrk:
	movl	__cbreak,r0	|* calcula o novo valor
	addl	r0,4(sp)

	movl	#BRK,r0		|* executa o pedido
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	movl	__cbreak,r0	|* devolve o "break" antigo
	movl	4(sp),r1	|* guarda o novo valor
	movl	r1,__cbreak
	rts

|*
|******	Alteração absoluta **************************************
|*
_brk:
	movl	#BRK,r0		|* executa o pedido
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	movl	4(sp),r1	|* guarda o novo valor e retorna 0
	movl	r1,__cbreak
	clrl	r0
	rts

|*
|******	_cbreak contem sempre o final da área corrente **********
|*
	.data
	.global	__cbreak
__cbreak:
	.long	0		|* Inicializado por "crt0"
