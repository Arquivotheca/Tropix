/*
 ****************************************************************
 *								*
 *			<math.h>				*
 *								*
 *	Variáveis e constantes matemáticas			*
 *								*
 *	Versão	1.0.0, de 22.11.87				*
 *		3.0.0, de 18.02.96				*
 *								*
 *	Módulo: math.h						*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1996 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#ifndef	MATH_H
#define	MATH_H

/*
 ******	Definição de macros *************************************
 */
#define M_PI	 	3.1415926535897932384626433	/* PI		*/
#define	M_PI_2	 	1.5707963267948966192313216	/* PI/2		*/
#define	M_PI_4	 	0.7853981633974483096156608	/* PI/4		*/

#define	M_2_PI	 	0.6366197723675813430755350 	/* 2/PI		*/      
#define	M_4_PI	 	1.2732395447351626861510700	/* 4/PI		*/

#define M_SQRT2		1.4142135623730950488016887	/* SQRT(2) 	*/
#define M_1_SQRT2	0.7071067811865475244008443	/* 1/SQRT(2) 	*/
#define M_SQRT2_2	0.7071067811865475244008443	/* SQRT(2)/2 	*/

#define M_E		2.7182818284590452353602874	/* E		*/

#define	M_LOG2E		1.4426950408889634073599246	/* LOG2 (E)	*/
#define	M_LOG210	3.3219280948873623478703194	/* LOG2 (10)	*/

#define	M_LOG102	0.3010299956639811952137388	/* LOG10 (2)	*/
#define	M_LOG10E	0.4342944819032518276511289	/* LOG10 (E)	*/

#define M_LOGE2		0.6931471805599453094172321	/* LOGE (2)	*/
#define M_LOGE10	2.3025850929940456840179914	/* LOGE (10)	*/

#define M_LN2		0.6931471805599453094172321	/* LOGE (2)	*/
#define M_LN10		2.3025850929940456840179914	/* LOGE (10)	*/

#define	M_1_SQRTPI	0.5641895835477562869480794	/* 1/SQRT(PI)	*/
#define	M_2_SQRTPI	1.1283791670955125738961588	/* 2/SQRT(PI)	*/

/*
 ******	Definição de tipos **************************************
 */
typedef struct	exception
{
	int	type;		/* x */
	char	*name;		/* x */
	double	arg1;		/* x */
	double	arg2;		/* x */
	double	retval;		/* x */

}	EXCEPTION;

/*
 ******	Variáveis globais ***************************************
 */
extern double	HUGE_VAL;		/* INFINITO */

/*
 ******	Protótipos de funções ***********************************
 */
extern double	acos (double);		/* Arco coseno */
extern double	asin (double);		/* Arco seno */
extern double	atan (double);		/* Arco tangente */
extern double	atan2 (double, double);	/* Arco tangente 2 */
extern double	cos (double);		/* Coseno */
extern double	sin (double);		/* Seno */
extern double	tan (double);		/* Tangente */
extern double	cosh (double);		/* Coseno hiperbólico */
extern double	sinh (double);		/* Seno hiperbólico */
extern double	tanh (double);		/* Tangente hiperbólica */
extern double	exp (double);		/* Exponencial */
extern double	exp2 (double);		/* Potência base 2 */
extern double	exp10 (double);		/* Potência base 10 */
extern double	expb (double, double);	/* Potência base dada */
extern double	pow (double, double);	/* Potencia base dada */
extern double	frexp (double, int *);	/* Fração + potência de 2 */
extern double	ldexp (double, int);	/* Produto por potência de 2 */
extern double	log (double);		/* Logaritmo natural */
extern double	log2 (double);		/* Logaritmo binário */
extern double	log10 (double);		/* Logaritmo decimal */
extern double	logb (double, double);	/* Logaritmo dada a base */
extern double	modf (double, double *); /* Parte integral + fracionária */
extern double	sqrt (double);		/* Raiz quadrada */
extern double	ceil (double);		/* Inteiro mais próximo não menor */
extern double	fabs (double);		/* Valor absoluto */
extern double	floor (double);		/* Inteiro mais próximo não maior */
extern double	fmod (double, double);	/* Resto flutuante */
extern double	hypot (double, double);	/* Distância euclidiana */
extern double	fgman (double);		/* Mantissa */
extern int	fgexp (double);		/* Expoente */
extern double	faexp (int, double);	/* Soma ao expoente */
extern double	fgint (double);		/* Parte inteira */
extern double	fgfra (double);		/* Parte fracionária */

/*
 ******	Rotinas para o manuseio de exceções do FPU do 486 ******
 */
extern long	fexcep (int);		/* liga/desliga exceções no 486 */
extern long	_fgsr (void);		/* obtém o valor do Status Reg. */
extern long	_fssr (long);		/* armazena um valor no Status Reg. */
extern long	_fgcr (void);		/* obtém o valor do Control Reg. */
extern long	_fscr (long);		/* armazena um valor no Control Reg. */
extern long	_fgiar (void);		/* obtém o valor do Instr. Addr. Reg. */

extern double	fhuge (void);		/* Valor infinito */
extern void	smatherr (EXCEPTION *);	/* Manuseio de erros */
extern int	matherr (EXCEPTION *);	/* Manuseio de erros */

/*
 ******	Definições para o manuseio de condições de erro ********
 */
#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

/*
 ******	Definições para o manuseio de exceções da FPU do 486 ***
 */
#define FP_INVALOPER	0x0001	/* Operação inválida */
#define FP_DENORMAL	0x0002	/* Operando não normalizado */
#define FP_ZERODIV	0x0004	/* Divisão por zero */
#define FP_OVERFLOW	0x0008	/* Overflow */
#define FP_UNDERFLOW	0x0010	/* Underflow */
#define FP_PRECISION	0x0020	/* Perda de Precisão */

#endif
