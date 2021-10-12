|*
|****************************************************************
|*								*
|*			write.s					*
|*								*
|*	Escreve em um arquivo					*
|*								*
|*	Versão	1.0.0, de 02.02.87				*
|*		2.3.0, de 09.09.88				*
|*								*
|*	Modulo: write						*
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
|*	write (fd, area, count)
|*	int   fd;
|*	char  *area;
|*	int   count
|*
|*	Retorna = Sucesso: bytes escritos (>=0); Erro: -1
|*
	.global	_write, _errno
 
WRITE = 4

_write:
	moveq	#WRITE,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
