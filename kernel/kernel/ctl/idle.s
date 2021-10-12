|*
|****************************************************************
|*								*
|*			idle.s					*
|*								*
|*	Processamento de IDLE (quando não tem o que fazer)	*
|*								*
|*	Versão	3.0.0, de 31.08.94				*
|*		4.9.0, de 09.08.06				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|******	Definições globais **************************************
|*
.if	IDLE_PATTERN

IDLE_PAT_LEN	= 16
IDLE_PAT_SPEED	= 6
IDLE_VIDEOADDR	= VGA_ADDR+2*[LINE*COL+COL-IDLE_PAT_LEN]

	.data

	.global	_idle_intr_active
_idle_intr_active:
	.long	1

	.global	_idle_color
_idle_color:
	.long	0x0100			|* 1 a 7

_idle_count:
	.long	0

_idle_ptr:
	.long	_idle_pattern

	.global	_idle_pattern
_idle_pattern:
	.iso	"********        "
	.iso	" ********       "
	.iso	"  ********      "
	.iso	"   ********     "
	.iso	"    ********    "
	.iso	"     ********   "
	.iso	"      ********  "
	.iso	"       ******** "
	.iso	"        ********"
	.iso	"*        *******"
	.iso	"**        ******"
	.iso	"***        *****"
	.iso	"****        ****"
	.iso	"*****        ***"
	.iso	"******        **"
	.iso	"*******        *"

	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "
	.iso	"\0               "

	.iso	"\0               "

.endif	IDLE_PATTERN

|*
|****** Avança e escreve o padrão *******************************
|*
	.text
	.global	_idle, _stopaddr
_idle:
.if	IDLE_PATTERN

	cmpl	#0,_idle_intr_active
	jz	9$

	pushl	r5
	pushl	r4

	decl	_idle_count
	jgt	1$

	movl	#IDLE_PAT_SPEED,_idle_count
	addl	#IDLE_PAT_LEN,_idle_ptr
1$:
	movl	_idle_ptr,r4
	cmpb	#0,(r4)
	jne	2$
	movl 	#_idle_pattern,r4
2$:
	movl	r4,_idle_ptr

	movl	#IDLE_VIDEOADDR, r5

	movl	#IDLE_PAT_LEN,r1
	clrl	r2
	movl	_idle_color,r0
5$:
	movb	(r4,r2),r0
	movw	r0,(r5,r2@w)

	incl	r2
	loop	5$

	popl	r4
	popl	r5

9$:
.endif	IDLE_PATTERN

	hlt
_stopaddr:

	rts

|*
|****** Troca a cor *********************************************
|*
	.global	_idle_inc_color
_idle_inc_color:
.if	IDLE_PATTERN
	movl	_idle_color,r0
	addl	#0x0100,r0
	cmpl	#0x0700,r0		|* 1 a 7
	jle	3$

	movl	#0x0100,r0
3$:
	movl	r0,_idle_color
.endif	IDLE_PATTERN
	rts

|*
|****************************************************************
|*	Espera um número determinado de microsegundos		*
|****************************************************************
|*
	.text
	.global	_DELAY
_DELAY:
	movl	4(sp),r2		|* O No. de useg
	movl	_DELAY_value,r1		|* O Valor para 1 useg
1$:
	subl	#1,r2
	jlt	9$

	movl	r1,r0
3$:
	subl	#1,r0
	jge	3$

	jmp	1$
9$:
	rts

.if 0	|*******************************************************|
|*
|****************************************************************
|*	Espera um número determinado de microsegundos		*
|****************************************************************
|*
	.text
	.global	_DELAY
_DELAY:
	movl	4(sp),r2		|* O No. de useg
	movl	_DELAY_value,r1		|* O Valor para 1 useg
1$:
	decl	r2
	jmi	9$

	movl	r1,r0
3$:
	decl	r0
	jpl	3$

	jmp	1$
9$:
	rts

|*
|****************************************************************
|*	Regra de 3 com arredondamento (64 bits)			*
|****************************************************************
|*
|*	ulong mul_div_64 (ulong val, ulong num, ulong denom);
|*
|*	deslocamento:	     4		8	    12
|*
|*
	.text
	.global	_mul_div_64
_mul_div_64:
	movl	4(sp),r0		|* Multiplica 
	mulul	8(sp),r0

	movl	12(sp),r1		|* Prepara o arredondamento
	lsrl	#1,r1

	addl	r1,r0			|* Arredonda
	adcl	#0,r2

	divul	12(sp),r0		|* Divide

	rts
.endif	|*******************************************************|
