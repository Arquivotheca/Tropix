|*
|****************************************************************
|*								*
|*			atan.s					*
|*								*
|*	Rotina para o cálculo do arco-tangente			*
|*								*
|*	Versão	3.0.0, de 07.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas:						*
|*		atan,	atan2					*
|*								*
|*	Módulo: atan						*
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
|*	Rotina que calcula o arco-tangente dado o argumento	* 
|****************************************************************
|*
|*	double	atan (double x);	retorna [-PI/2, +PI/2]
|*
	.global	_atan
_atan:
	fldd	4(sp)		|* Argumento em f0
	fld1			|* 1 em f0 e arg. pushed para f1
	fpatan
	rts

|*
|****************************************************************
|*								*
|*	Rotina que calcula o arco-tangente dados dois		* 
|*	 		argumentos (y, x)			*
|* 								*
|* 		y > 0, x > 0	-> 1. quadrante			*
|* 		y > 0, x < 0	-> 2. quadrante			*
|* 		y < 0, x > 0	-> 3. quadrante			*
|* 		y < 0, x < 0	-> 4. quadrante			*
|* 								*
|****************************************************************
|*
|*	double	atan2 (double y, double x);	retorna [-PI, +PI]
|*
	.global	_atan2
_atan2:
	fldd	4(sp)		|* y em f0
	fldd	12(sp)		|* x em f0 e y pushed para f1
	fpatan
	rts
