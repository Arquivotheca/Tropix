/*
 ****************************************************************
 *								*
 *			pottb.c					*
 *								*
 *	Tabela de reais que são potências de 10			*
 *								*
 *	Versão	3.0.0, de 08.07.95				*
 *		3.0.0, de 18.07.95				*
 *								*
 *	Módulo: pottb						*
 *		libm/conv					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#undef	ASM

/*
 ****************************************************************
 *	Tabela de potências para a conversão			*
 ****************************************************************
 */
#ifdef	ASM

typedef	struct
{
	long	e_pot0, e_pot1;	/* Potência */
	int	e_exp;		/* Expoente */
	
}	POTTB;
 
const  POTTB	_pottb[] =
{
	0x00000000,	0x40240000, 	1,
	0x00000000,	0x40590000, 	2,
	0x00000000,	0x40C38800, 	4,
	0x00000000,	0x4197D784, 	8,
	0x37E08000,	0x4341C379, 	16,
	0xB5056E17,	0x4693B8B5, 	32,
	0xE93FF9F5,	0x4D384F03, 	64,
	0xF9301D32,	0x5A827748, 	128,
	0x7F73BF3C,	0x75154FDD, 	256
};

#else

typedef	struct
{
	double	e_pot;		/* Potência */
	int	e_exp;		/* Expoente */
	
}	POTTB;
 
const  POTTB	_pottb[] =
{
	1.0E+1,		1,
	1.0E+2,		2,
	1.0E+4,		4,
	1.0E+8,		8,
	1.0E+16,	16,
	1.0E+32,	32,
	1.0E+64,	64,
	1.0E+128,	128,
	1.0E+256,	256
};

#endif	ASM

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Outras Constantes					*
 ****************************************************************
 */
const double	ZERO_PT_ZERO = 0.0;
const double	UM_PT_ZERO = 1.0;
const double	DEZ_PT_ZERO = 10.0;
const double	ZERO_PT_ZERO_TRES = 0.03;

	.global	_ZERO_PT_ZERO
_ZERO_PT_ZERO:
	.long	0x00000000,0x00000000

	.global	_UM_PT_ZERO
_UM_PT_ZERO:
	.long	0x00000000,0x3FF00000

	.global	_DEZ_PT_ZERO
_DEZ_PT_ZERO:
	.long	0x00000000,0x40240000

	.global	_ZERO_PT_ZERO_TRES
_ZERO_PT_ZERO_TRES:
	.long	0xEB851EB8,0x3F9EB851

	.global	_HUGE_VAL
_HUGE_VAL:
	.long	0xFFFFFFFF,0x7FEFFFFF
#endif	/*******************************************************/
