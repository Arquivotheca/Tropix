|*
|****************************************************************
|*								*
|*			kill.s					*
|*								*
|*	Envia um sinal para processos				*
|*								*
|*	Versão	1.0.0, de 16.01.86				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: kill						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*
|*

|*
|*	int
|*	kill (pid, sig)
|*	int pid;
|*	int sig;
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_kill, _errno
 
KILL = 37
 
_kill:
	moveq	#KILL,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
