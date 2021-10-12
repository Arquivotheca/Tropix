|*
|****************************************************************
|*								*
|*			brk.s					*
|*								*
|*	Muda o tamanho do segmento de dados do processo		*
|*								*
|*	Versão	1.0.0, de 08.01.87				*
|*		2.3.0, de 02.09.88				*
|*								*
|*	Módulo: brk.s						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|*	int
|*	brk (addr)
|*	char *addr;
|*
|*	retorno => sucesso: 0, erro: -1
|*
|*	char *
|*	sbrk (incr)
|*	int incr;
|*
|*	retorno => sucesso: "break" antigo, erro: -1 
|*
	.global	_sbrk, _brk
	.global	_errno
 
BRK = 17 

|*
|*	Alteração diferencial
|*
_sbrk:
	movl	__cbreak,d0	|* calcula o novo valor
	addl	d0,4(sp)

	moveq	#BRK,d0		|* executa o pedido
	movl	4(sp),a0

	trap	#0
	jcs	erro

	movl	__cbreak,d0	|* devolve o "break" antigo
	movl	4(sp),__cbreak	|* guarda o novo valor
	rts
 
erro:
	movl	d0,_errno	|* guarda o no. do erro e devolve -1
	moveq	#-1,d0
	rts
 
|*
|*	Alteração absoluta
|*
_brk:
	moveq	#BRK,d0		|* executa o pedido
	movl	4(sp),a0

	trap	#0
	jcs	erro

	movl	4(sp),__cbreak	|* guarda o valor novo e retorna 0
	clrl	d0
	rts

|*
|*	_cbreak contem sempre o final da area de segmentos corrente. 
|*
	.data
	.global	__cbreak, _end

__cbreak:
	.long	_end
