|*
|****************************************************************
|*								*
|*			boot.s					*
|*								*
|*	Reinicializa o computador				*
|*								*
|*	Versão	3.0.0, de 01.09.97				*
|*		3.0.0, de 01.09.97				*
|*								*
|*	Modulo: boot						*
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
|*	boot ()
|*
|*	Somente retorna em caso de erro
|*
	.global	_boot, _errno
 
BOOT = 77
 
_boot:
	moveq	#BOOT,d0
	trap	#0

	bras	1$		|* Retorno do Filho
	jcs	2$		|* Retorno do Pai
	rts
1$:
	rts			|* Filho
2$:
	movl	d0,_errno	|* Pai com Erro
	moveq	#-1,d0
	rts 
