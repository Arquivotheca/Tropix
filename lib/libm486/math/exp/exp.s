|*
|****************************************************************
|*								*
|*			exp.s					*
|*								*
|*	Rotina para o cálculo da exponencial			*
|*								*
|*	Versão	3.0.0, de 09.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas:						*
|*		exp,	exp2,	exp10,	expb,	pow		*
|*								*
|*	Módulo: exp						*
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
|*	Rotina que calcula	(e ** arg)			* 
|****************************************************************
|*
|*	double	exp (double arg);
|*
	.global	_exp
_exp:
	fldd	4(sp)		|* Carrega o argumento
	fldl2e			|* Carrega log na base 2 de 10
	fmulp			|* Multiplica o argumento
				|* Daqui em diante igual a 2 ** arg
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	trunc		|* Força arredondamento por truncamento
	fld	f0		|* f1 = f0 = argumento
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	fsubr	f1		|* f0 com a parte fracionária
				|* f1 com o argumento
	f2xm1			|* f0 =  2 ** frac - 1
	fld1
	faddp			|* f0 =  2 ** frac
	fscale			|* Soma no exp. a parte inteira do arg.
	fstp	f1		|* Pop resultado (ajuste de stack)
	rts

|*
|****************************************************************
|*	Rotina que calcula	(2 ** arg)			* 
|****************************************************************
|*
|*	double	exp2 (double arg);
|*
	.global	_exp2
_exp2:
	fldd	4(sp)		|* Carrega o argumento
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	trunc		|* Força arredondamento por truncamento
	fld	f0		|* f1 = f0 = argumento
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	fsubr	f1		|* f0 com a parte fracionária
				|* f1 com o argumento
	f2xm1			|* f0 =  2 ** frac - 1
	fld1
	faddp			|* f0 =  2 ** frac
	fscale			|* Soma no exp. a parte inteira do arg.
	fstp	f1		|* Pop resultado (ajuste de stack)
	rts

|*
|****************************************************************
|*	Rotina que calcula	(10 ** arg)			* 
|****************************************************************
|*
|*	double	exp10 (double arg);
|*
	.global	_exp10
_exp10:
	fldd	4(sp)		|* Carrega o argumento
	fldl2t			|* Carrega log na base 2 de 10
	fmulp			|* Multiplica o argumento
				|* Daqui em diante igual a 2 ** arg
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	trunc		|* Força arredondamento por truncamento
	fld	f0		|* f1 = f0 = argumento
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	fsubr	f1		|* f0 com a parte fracionária
				|* f1 com o argumento
	f2xm1			|* f0 =  2 ** frac - 1
	fld1
	faddp			|* f0 =  2 ** frac
	fscale			|* Soma no exp. a parte inteira do arg.
	fstp	f1		|* Pop resultado (ajuste de stack)
	rts
	
|*
|****************************************************************
|*	Rotina que calcula	(base ** arg)			* 
|****************************************************************
|*
|*	double	expb (double base, double arg);
|*	double	pow (double base, double arg);
|*
	.global	_expb, _pow
_expb:
_pow:
	fldd	12(sp)		|* carrega argumento (y)
	fldd	4(sp)		|* carrega base (x)
	fyl2x			|* y * log na base 2 de x
				|* Daqui em diante igual a 2 ** arg
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	trunc		|* Força arredondamento por truncamento
	fld	f0		|* f1 = f0 = argumento
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	fsubr	f1		|* f0 com a parte fracionária
				|* f1 com o argumento
	f2xm1			|* f0 =  2 ** frac - 1
	fld1
	faddp			|* f0 =  2 ** frac
	fscale			|* Soma no exp. a parte inteira do arg.
	fstp	f1		|* Pop resultado (ajuste de stack)
	rts
	
|*
|****************************************************************
|*	Constante necessária para o tipo de arredondamento	* 
|****************************************************************
|*
	.const
trunc:
	.word	0x0F7F		|* arredondamento para zero 
				|* (truncamento)
