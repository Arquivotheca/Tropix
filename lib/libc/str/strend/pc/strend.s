|*
|****************************************************************
|*								*
|*			strend.s				*
|*								*
|*	Obtém o final de uma cadeia				*
|*								*
|*	Versão	3.0.0, de 19.12.94				*
|*		3.0.0, de 03.01.95				*
|*								*
|*	Módulo: strend						*
|*		libc/str					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*


|*
|****************************************************************
|*	Obtém o final de uma cadeia				*
|****************************************************************
|*
|*	char *
|*	strend (const char *src)
|*
	.global	_strend
_strend:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "src"
	movl	#-1,r1		|* Maior valor possível para o contador
	clrl	r0		|* Valor = 0

	up			|* Testa a cadeia
	repne
	scasb

	movl	r5,r0		|* Passa um caractere
	decl	r0

	popl	r5
	unlk
	rts
