|*
|****************************************************************
|*								*
|*			floor.c					*
|*								*
|*	Miscelânea de rotinas matemáticas			*
|*								*
|*	Versão	3.0.0, de 07.02.96				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas:						*
|*		floor	ceil	fmod	fabs			*
|*								*
|*	Módulo: floor						*
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
|*	Rotina que calcula inteiro <= arg			* 
|****************************************************************
|*
|*	double	floor (double arg);
|*
	.global	_floor
_floor:
	fldd	4(sp)		|* Carrega o argumento
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	menosinf	|* Força arredondamento para -infinito
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	rts

|*
|****************************************************************
|*	Rotina que calcula inteiro >= arg			* 
|****************************************************************
|*
|*	double	ceil (double arg);
|*
	.global	_ceil
_ceil:
	fldd	4(sp)		|* Carrega o argumento
	fstcw	-4(sp)		|* Armazena tipo de arredondamento
	fldcw	maisinf		|* Força arredondamento para +infinito
	frndint			|* Extrai a parte inteira em f0
	fldcw	-4(sp)		|* Restaura tipo de arredondamento
	rts

|*
|****************************************************************
|*	Rotina que calcula o resto da divisao de x por y	* 
|****************************************************************
|*
|*	double	fmod (double x, double y);
|*
	.global	_fmod
_fmod:
	fldd	12(sp)
	fabs
	fldd	4(sp)
1$:
	fprem
	fstsw
	sahf
	jp	1$	|* testa se a operação terminou
	rts

|*
|****************************************************************
|*	Rotina que calcula o valor absoluto			* 
|****************************************************************
|*
|*	double	fabs (double a);
|*
	.global	_fabs
_fabs:
	fldd	4(sp)
	fabs
	rts

|*
|****************************************************************
|*	Constantes necessárias para o tipo de arredondamento	* 
|****************************************************************
|*
	.const
maisinf:
	.word	0x0B7F		|* arredonda para mais infinito

menosinf:
	.word	0x077F		|* arredonda para menos infinito
