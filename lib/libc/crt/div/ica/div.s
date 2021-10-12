|*
|****************************************************************
|*								*
|*			div.s					*
|*								*
|*	Quociente e Resto da divisão de	dois inteiros		*
|*		longos com sinal				*
|*								*
|*	Versão	3.0.0, de 18.03.90				*
|*		3.0.0, de 18.03.90				*
|*								*
|*	Módulo: div						*
|*		libc/crt					*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************

|*
|*	#include <stdlib.h>
|*
|*	div_t  div  (int  numer, int  denom);
|*	ldiv_t ldiv (long numer, long denom);
|*
	.global	_div, _ldiv
_div:
_ldiv:
	tstw	4(sp)		|* testa se dividendo > 0 e < (2 ** 16)
	jeq	1$
	cmpw	#0xFFFF,4(sp)	|* testa se dividendo < 0 e > -(2 ** 16)
	jne	3$
1$:
	tstw	8(sp)		|* testa se divisor > 0 e < (2 ** 16)
	jeq	2$
	cmpw	#0xFFFF,8(sp)	|* testa se divisor < 0 e > -(2 ** 16)
	jne	3$
2$:
	movl	4(sp),d0	|* divisão curta
	divs	10(sp),d0
	movl	d0,d1
	extl	d0		|* posiciona o sinal do quociente
	swap	d1		|* posiciona o resto
	extl	d1		|* posiciona o sinal do resto
	rts

3$:
	moveml	#<d2,d3,d4>,-(sp)

	movl	16(sp),d3	|* carrega dividendo
	jge	4$
	negl	d3		|* faz dividendo positivo
4$:
	movl	d3,d1
	movl	20(sp),d4	|* carrega divisor
	jge	5$
	negl	d4		|* faz divisor positivo
5$:
	movl	d4,d2
	cmpl	#0x00010000,d4
	jhs	6$		|* divisor maior que (2 ** 16)

	clrw	d1		|* zera parte baixa do dividendo
	swap	d1
	divu	d4,d1		|* a(alta) / b(baixa)
	movw	d1,d0		|* guarda quociente parcial
	swap	d0
	movw	d3,d1
	divu	d4,d1		|* resto e a(baixa) / b(baixa)
	movw	d1,d0		|* finaliza o quociente
	clrw	d1
	swap	d1		|* posiciona o resto
	jra	8$
6$:
	lsrl	#1,d3		|* divide o dividendo e
	lsrl	#1,d4		|* o divisor por 2
	cmpl	#0x00010000,d4	|* até divisor < (2 ** 16)
	jhs	6$

	divu	d4,d3
	andl	#0x0000FFFF,d3	|* apaga o resto da divisão
	movl	d3,d0		|* guarda o quociente

	mulu	d2,d3		|* multiplica o quociente
	movl	d2,d4		|* pelo divisor
	swap	d4
	mulu	d0,d4		|* em duas partes
	swap	d4
	addl	d4,d3		|* obtendo o resultado em d3

	cmpl	d1,d3		|* compara resultado com o dividendo
	jlo	7$		|* se resultado é menor

	subql	#1,d0		|* subtrai um do quociente em d0
	subl	d2,d3		|* subtrai o divisor do resultado
7$:
	subl	d3,d1		|* subtrai o resultado do dividendo
				|* dando o resto em d1
8$:
	movw	16(sp),d3	|* ajusta o sinal do resto
	jge	9$
	negl	d1
9$:
	eorw	d3,20(sp)	|* ajusta o sinal do quociente
	jge	10$
	negl	d0
10$:
	moveml	(sp)+,#<d2,d3,d4>
	rts
