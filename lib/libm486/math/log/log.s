|*
|****************************************************************
|*								*
|*			log.s					*
|*								*
|*	Rotina para o cálculo de logaritmos			*
|*								*
|*	Versão	3.0.0, de 05.08.95				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas: 						*
|*		log,	log2,	log10,	logb			*
|*								*
|*	Módulo: log						*
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
|*	Rotina que calcula o logaritmo na base e de "arg"	* 
|****************************************************************
|*
|*	double	log (double arg);
|*
	.global	_log
_log:
	fldln2		|* Log de 2 na base e na stack
	fldd	4(sp)	|* Número na stack
	fyl2x		|* Log do número na base e na stack
	rts

|*
|****************************************************************
|*	Rotina que calcula o logaritmo na base 2 de "arg"	* 
|****************************************************************
|*
|*	double	log2 (double arg);
|*
	.global	_log2
_log2:
	fld1		|* Log de 2 na base 2 na stack
	fldd	4(sp)	|* Número na stack
	fyl2x		|* Log do número na base 2 na stack
	rts

|*
|****************************************************************
|*	Rotina que calcula o logaritmo na base 10 de "arg"	* 
|****************************************************************
|*
|*	double	log10 (double arg);
|*
	.global	_log10
_log10:
	fldlg2		|* Log de 2 na base 10 na stack
	fldd	4(sp)	|* Número na stack
	fyl2x		|* Log do número na base 10 na stack
	rts

|*
|****************************************************************
|*	Rotina que calcula o logaritmo na base "base" de "arg"	* 
|****************************************************************
|*
|*	double	logb (double base, double expt);
|*
	.global	_logb
_logb:
	fld1		|* Põe dois uns na stack
	fld1
	fldd	4(sp)
	fyl2x		|* Log de "base" na base 2 na stack
	fdivp		|* Log de 2 na base "base" na stack
	fldd	12(sp)
	fyl2x		|* Log do número na base "base" na stack
	rts
