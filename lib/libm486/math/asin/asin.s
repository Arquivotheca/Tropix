|*
|****************************************************************
|*								*
|*			asin.s					*
|*								*
|*	Rotina para o cálculo do arco-seno e arco-cosseno	*
|*								*
|*	Versão	3.0.0, de 07.02.96				*
|*		3.0.0, de 07.02.96				*
|*								*
|*	Rotinas:						*
|*		asin,	acos					*
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
|*	Rotina que calcula o arco-seno dado o argumento 	* 
|****************************************************************
|*
|*	double	asin (double x);	retorna [-PI/2, +PI/2]
|*
	.global	_asin
_asin:
	fldd	4(sp)		|* Push seno
	fld1			|* Push 1
	fld	f1		|* Push seno 
	fld	f0		|* Push seno
	fmulp			|* Seno ao quadrado em f0
	fsubp			|* 1 - seno ao quadrado em f0
	fsqrt			|* cosseno em f0 com seno em f1
	fpatan
	rts

|*
|****************************************************************
|*	Rotina que calcula o arco-cosseno dado o argumento	*
|****************************************************************
|*
|*	double	acos (double x);	retorna [0, +PI]
|*
	.global	_acos
_acos:
	fldd	4(sp)		|* Push cosseno
	fld1			|* Push 1
	fld	f1		|* Push cosseno 
	fld	f0		|* Push cosseno
	fmulp			|* Cosseno ao quadrado em f0
	fsubp			|* 1 - cosseno ao quadrado em f0
	fsqrt			|* Seno em f0 com cosseno em f1
	fxch			|* Cosseno em f0 com seno em f1	
	fpatan
	rts
