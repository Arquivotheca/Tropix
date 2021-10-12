|*
|****************************************************************
|*								*
|*			shmem.s					*
|*								*
|*	Obtém regiões de memória compartilhada para o usuário	*
|*								*
|*	Versão	1.0.0, de 19.10.90				*
|*		2.3.0, de 18.11.90				*
|*								*
|*	Modulo: shmem						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|* 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	#include <sys/types.h>
|*	#include <sys/usync.h>
|*
|*	int
|*	shmemctl (REGIONCTLFUNC func, int fd, int arg1)
|*
|*	func = RG_ALLOC: arg1 = no. de regiões
|*	Retorna = Sucesso: 0; Erro: -1
|*
|*	func = RG_GET:   arg1 = id. da região
|*	Retorna = Sucesso: Endereço virtual da região; Erro: -1
|*
	.global	_shmemctl, _errno
 
SHMEMCTL = 117

_shmemctl:
	movl	d2,-(sp)

	moveq	#SHMEMCTL,d0
	movl	8(sp),a0
	movl	12(sp),d1
	movl	16(sp),a1
	movl	20(sp),d2

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	movl	(sp)+,d2
	rts

