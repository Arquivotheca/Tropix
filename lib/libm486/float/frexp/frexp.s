|*
|****************************************************************
|*								*
|*			frexp.s					*
|*								*
|*	Rotinas para manuseio em separado da mantissa		*
|*	e do expoente de números de ponto flutuante		*
|*								*
|*	Versão	3.0.0, de 15.07.95				*
|*		3.0.0, de 11.03.96				*
|*								*
|*	Rotinas:						*
|*		frexp,	ldexp,	modf				*
|*								*
|*	Módulo: frexp						*
|*		libm486/float					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Retorna a mantissa e armazena o expoente		* 
|*	num endereco dado					*
|****************************************************************
|*
|*	double	frexp (double a, int *expaddr);
|*	
	.global	_frexp
_frexp:
	fldd	4(sp)
	fxtract			|* Mantissa em f0 e expoente em f1
	fxch			|* Expoente em f0 e mantissa em f1
	movl	12(sp),r0	|* Endereço em r0
	fistpl	(r0)		|* Expoente inteiro na memória
	rts

|*
|****************************************************************
|*	Soma um valor inteiro ao expoente, i.e.,		*
|*		multiplica por uma potência de 2		*
|****************************************************************
|*
|*	double	ldexp (double a, int inc);
|*
	.global	_ldexp
_ldexp:
	fildl	12(sp)		|* Carrega o incremento (inteiro)
	fldd	4(sp)		|* Carrega o operando
	fscale			|* Incrementa o expoente
	rts

|*
|****************************************************************
|*	Retorna a parte fracionaria (double) e armazena a	*
|*	parte inteira (double) num endereco dado		*
|****************************************************************
|*
|*	double	modf (double frac, double intaddr);
|*
	.global	_modf
_modf:
	fstcw	-4(sp)
	fldcw	trunc
	fldd	4(sp)		|* Carrega o operando
	frndint			|* Extrai inteiro
	movl	12(sp),r0	|* Endereço da parte inteira em r0
	fstd	(r0)		|* Parte inteira na memória
	fsubd	4(sp)		|* Subtrai o número da parte inteira
	fchs			|* Sobra a parte fracionária
				|* com sinal invertido
	fldcw	-4(sp)
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
