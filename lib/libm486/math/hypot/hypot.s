|*
|****************************************************************
|*								*
|*			hypot.s					*
|*								*
|*	Rotina para o cálculo da hipotenusa (dist. Euclideana)	*
|*								*
|*	Versão	3.0.0, de 09.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Módulo: hypot						*
|*		libm486/math					*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*								*
|*	Rotina usando a formula de Pitagoras mas evitando	*
|*		um "overflow" contornável			*
|*								*
|*	a = SQRT ((b ** 2) + (c ** 2))				*
|*								*
|*	(a / b) = SQRT ((1 + (c / b) ** 2))			*
|*								*
|*	a = b * SQRT ((1 + (c / b) ** 2))			*
|*								*
|****************************************************************
|*
 
|*
|****************************************************************
|*	Rotina que calcula a Hipotenusa				* 
|****************************************************************
|*
|*	double	hypot (double b, double c);
|*
	.global	_hypot
_hypot:
	fldd	4(sp)		|* push b
	fabs
	fldd	12(sp)		|* push c
	fabs
	fld	f0		|* push c, novamente
				|* agora: c em f0 e f1; b em f2
	fadd	f2,f0
	fcomp	f0,f2		|* if ((b + c) == b) retorna b
	fstsw
	sahf
	jne	1$		|* agora: c em f0 e b em f1
	fxch			|* carrega b
	fstp	f1
	rts
1$:
	fld	f1		|* push b em f0
	fadd	f1,f0
	fcomp	f0,f1		|* if ((b + c) == b) retorna c
	fstsw
	sahf
	jne	2$		|* agora: c em f0 e b em f1
	fstp	f1
	rts
2$:
	fdiv	f1		|* t = (c / b); b ainda em f1
	fld	f0
	fmulp
	fld1
	faddp
	fsqrt
	fmulp			|* t = (b * sqrt (1.0 + t * t))
	rts
