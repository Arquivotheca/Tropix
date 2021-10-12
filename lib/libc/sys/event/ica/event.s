|*
|****************************************************************
|*								*
|*			event.s					*
|*								*
|*	Obtém eventos para o usuário				*
|*								*
|*	Versão	1.0.0, de 19.10.90				*
|*		2.3.0, de 19.10.90				*
|*								*
|*	Modulo: event						*
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
|*	eventget (UEVENTCTLFUNC func, int fd, int arg2)
|*
|*	func = UE_ALLOC: arg2 = no. de eventos
|*	Retorna = Sucesso: 0; Erro: -1
|*
|*	func = UE_GET:   arg2 = id. do evento
|*	Retorna = Sucesso: Descritor do evento; Erro: -1
|*
	.global	_eventctl, _errno
 
EVENTCTL = 109

_eventctl:
	moveq	#EVENTCTL,d0
	movl	4(sp),a0
	movl	8(sp),d1
	movl	12(sp),a1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

|*
|*	int
|*	eventwait (int eventdescr, int eventno)
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventwait, _errno
 
EVENTWAIT = 110

_eventwait:
	moveq	#EVENTWAIT,d0
	movl	4(sp),a0
	movl	8(sp),d1

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

|*
|*	int
|*	eventdone (int eventdescr)
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventdone, _errno
 
EVENTDONE = 111

_eventdone:
	moveq	#EVENTDONE,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

|*
|*	int
|*	eventtest (int eventdescr)
|*
|*	Retorna = Sucesso: Número do evento mais recente; Erro: -1
|*
	.global	_eventtest, _errno
 
EVENTTEST = 112

_eventtest:
	moveq	#EVENTTEST,d0
	movl	4(sp),a0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts

