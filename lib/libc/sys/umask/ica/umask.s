|*
|****************************************************************
|*								*
|*			umask.s					*
|*								*
|*	Obtem e modifica a mascara de criação de arquivos	*
|*								*
|*	Versão	1.0.0, de 27.01.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: umask						*
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
|*	umask (cmask)
|*	int cmask;
|*
|*	Retorna = Sucesso: Valor antigo da mascara
|*
	.global	_umask
 
UMASK = 60

_umask:
	moveq	#UMASK,d0
	movl	4(sp),a0

	trap	#0
	rts
