|*
|****************************************************************
|*								*
|*			chdirip.s				*
|*								*
|*	Troca de diretorio dado o ponteiro do INODE		*
|*								*
|*	Versão	1.0.0, de 03.02.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: chdirip						*
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
|*	chdirip (ip)
|*	INODE	*ip;
|*
|*	Retorna = Sucesso: 0, erro: -1
|*
	.global	_chdirip, _errno
 
CHDIRIP = 98
 
_chdirip:
	moveq	#CHDIRIP,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
