|*
|****************************************************************
|*								*
|*			crt0.s					*
|*								*
|*	Prólogo para programas em "C"				*
|*								*
|*	Versão	1.0.0, de 25.11.86				*
|*		3.2.3, de 27.11.99				*
|*								*
|*	Módulo: crt0						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|******	Ponto de início de um programa **************************
|*
	.global	start
start:
	movl	sp,r0		|* Abre espaço para "argc" e "argv"
	subl	#8,sp

	movl	(r0),r1		|* Copia e guarda "argc"
	movl	r1,(sp)

	addl	#4,r0		|* Prepara "argv"
	movl	r0,4(sp)

	movl	(r0),r2		|* Guarda o argv[0]
	movl	r2,_pgname

	lsll	#2,r1		|* Calcula "envp"
	addl	r1,r0
	addl	#4,r0
	movl	r0,8(sp)

	movl	r0,_environ	|* Prepara o "ambiente"

	movl	#_end,r0	|* Inicializa "brk"
	movl	r0,__cbreak

	call	_main		|* Chama o Programa
   |***	addl	#12,sp

	pushl	r0		|* Encerra o Programa
	call	_exit
