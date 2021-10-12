|*
|****************************************************************
|*								*
|*			sin.s					*
|*								*
|*	Rotinas para o cálculo de Seno e Cosseno		*
|*								*
|*	Versão	3.0.0, de 03.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas: 						*
|*		sin,	cos					*
|*								*
|*	Módulo: sin						*
|*		libm/math					*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Rotina para cálculo do SENO				*
|****************************************************************
|*
|*	double	sin (double arg);
|*
	.global	_sin
_sin:
	fldd	4(sp)		|* Argumento na stack
	fsin
	rts

|*
|****************************************************************
|*	Rotina para cálculo do COSSENO				*
|****************************************************************
|*
|*	double	cos (double arg);
|*
	.global	_cos
_cos:
	fldd	4(sp)		|* Argumento na stack
	fcos
	rts
