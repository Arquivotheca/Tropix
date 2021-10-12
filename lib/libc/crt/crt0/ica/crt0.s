|*
|****************************************************************
|*								*
|*			crt0.s					*
|*								*
|*	Prólogo para programas em "C"				*
|*								*
|*	Versão	1.0.0, de 25.11.86				*
|*		2.3.0, de 26.09.88				*
|*								*
|*	Módulo: crt0						*
|*		libc/crt					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*
 
|*
|****************************************************************
|*	Definicoes Globais					*
|****************************************************************
|*
EXIT	= 1

|*
|****************************************************************
|*	Ponto de Inicio de um Programa				*
|****************************************************************
|*
	.global	start, _exit, _main
 
start:
	movl	sp,a0		|* Abre espaco para "argc" e "argv"
	subql	#8,sp

	movl	(a0),d0		|* Copia e guarda "argc"
	movl	d0,(sp)

	addql	#4,a0		|* Prepara "argv"
	movl	a0,4(sp)

	lsll	#2,d0		|* Calcula "envp"
	addl	d0,a0
	addql	#4,a0
	movl	a0,8(sp)

	movl	a0,_environ	|* Prepara o "Ambiente"

	jsr	_main		|* Chama o Programa
	addql	#8,sp

	movl	d0,-(sp)	|* Encerra o Programa
	jsr	_exit

	movw	#EXIT,d0	|* Caso de Panico
	trap	#0
 
|*
|****************************************************************
|*	Variaveis Globais					*
|****************************************************************
|*
	.bss
	.global	_errno, _environ

_environ:
	.long	0	|* Ponteiro do "Ambiente"

_errno:	
	.long 	0	|* Numero do erro de uma Chamada ao Sistema
