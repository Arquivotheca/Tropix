|*
|****************************************************************
|*								*
|*			spinlock.s				*
|*								*
|*	Semáforo de nivel baixo					*
|*								*
|*	Versão	3.0.0, de 04.10.94				*
|*		3.0.0, de 12.02.95				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

N	= 1000000000		|* Para dar um tempo razoável

|*
|****************************************************************
|*	Semáforo de recursos rápidos 				*
|****************************************************************
|*
|*	void spinlock (LOCK *lock);
|*
|*	Fica em loop enquanto não consegue dar o "tas" em "lock"
|*
	.global	_spinlock
_spinlock:
1$:
	call	_spl7		|* "spl7" retorna a PL anterior

	movl	4(sp),r2	|* Obtém o endereço do semáforo

	lock			|* Tenta o TAS
	btsl	#7,(r2)
	jcs	2$

|******	Estava livre: guarda a PL anterior e retorna

	orb	r0,(r2)
	rts

|******	Estava ocupado: restaura a prioridade e espera liberar
2$:
	pushl	r0
	call	_splx
	addl	#4,sp

	movl	#N,r1		|* Esperar liberar
4$:
	decl	r1
	jle	6$

	btl	#7,(r2)
	jcs	4$

	jra	1$

|******	Erro: Passou 1 segundo
6$:
	pushl	#format_0
	call	_printf
	addl	#4,sp

	jra	1$

|******	Mensagem de Erro

	.data
format_0:
	.asciz	"spinlock: Em loop: pc = %P, semáforo = %P\n"

|*
|****************************************************************
|*	Liberação de recursos rápidos				*
|****************************************************************
|*
|*	void spinfree (LOC *lock);
|*
|*	Restaura a prioridade original,	e libera o recurso
|*
	.text
	.globl	_spinfree
_spinfree:
	movl	4(sp),r2	|* Obtém o endereço do semáforo

	clrl	r0		|* Libera o semáforo
	xchgb	r0,(r2)

	tstb	r0		|* Verifica se estava ocupado
	jpl	6$

	andb	#0x7F,r0	|* Restaura a prioridade anterior
	pushl	r0
	call	_splx
	addl	#4,sp

	rts

|******	Erro: O semáforo estava desocupado
6$:
	pushl	#format_1
	call	_printf
	addl	#4,sp

	rts

|******	Mensagem de Erro

	.data
format_1:
	.asciz	"spinfree: Desocupado: pc = %P, semáforo = %P\n"

|*
|****************************************************************
|*	Função de "test & set"					*
|****************************************************************
|*
|*	ret = tas (LOCK *lock);
|*
|*	ret:  0 => Estava desocupado
|*	     -1 => Ja' estava ocupado
|*
	.text
	.global	_tas
_tas:
	movl	4(sp),r2	|* Obtém o endereço do semáforo

	clrl	r0

	lock			|* Tenta o TAS
	btsl	#7,(r2)
	jcc	1$		|* Estava livre: retorna "0"

	decl	r0		|* Estava ocupado: retorna "-1"
1$:
	rts
