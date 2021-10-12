|*
|****************************************************************
|*								*
|*			crt1.s					*
|*								*
|*	Continuação do prólogo para programas em "C"		*
|*								*
|*	Versão	3.2.3, de 25.11.99				*
|*		3.2.3, de 27.11.99				*
|*								*
|*	Módulo: crt1						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Para o caso de erro em chamadas ao sistema **************
|*
	.global	syscall_error
syscall_error:
	movl	r2,_errno
	movl	#-1,r0
	rts

|*
|******	Variáveis globais ***************************************
|*
	.bss
	.global	_errno, _environ, _pgname

_environ:
	.long	0	|* Ponteiro do "ambiente"

_errno:	
	.long 	0	|* Número do erro de uma chamada ao sistema

_pgname:	
	.long 	0	|* Caminho completo do programa
