|*
|****************************************************************
|*								*
|*			tan.c					*
|*								*
|*	Rotina para o cálculo da Tangente			*
|*								*
|*	Versão	3.0.0, de 03.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas: 						*
|*		tan						*
|*								*
|*	Módulo: tan						*
|*		libm486/math					*
|*		Categoria D					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Rotina que calcula a TANGENTE 				*
|****************************************************************
|*
|*	double	tan (double arg);
|*
	.global	_tan
_tan:
	fldd	4(sp)		|* Argumento na stack
	fptan			|* 1.0 em f0 e tangente em f1
	fxch			|* tangente em f0
	fstp	f1		|* ajusta a stack
	rts
