#include <math.h>
#include <stdio.h>
#include <signal.h>

void	fexcepid (int sig);

main ()
{
	int	i;
	int	x, y;
	double	a;
	double	b, c, d, e;

	signal (SIGFPE, fexcepid);

	y = (FP_INVALOPER | FP_DENORMAL | FP_ZERODIV |
		FP_OVERFLOW | FP_UNDERFLOW);

	printf ("Condições passíveis de gerar exceção:\n");
	printf ("\t1  -> Operação inválida\n");
	printf ("\t2  -> Operando não normalizado\n");
	printf ("\t4  -> Divisão por zero\n");
	printf ("\t8  -> Overflow\n");
	printf ("\t16 -> Underflow\n");
	printf ("\t32 -> Precisão\n");

	printf ("Dê o conjunto de condições desejadas: ");
	scanf ("%d", &y);

	x = fexcep (y);
	printf ("Exceções anteriores habilitadas: %04X\n", x);
	x = fexcep (y);
	printf ("Exceções pedidas: %04X\n", y);
	printf ("Exceções habilitadas: %04X\n", x);

	printf ("a < -307 ----> underflow em exp10\n");
	printf ("a > 308  ----> overflow em exp10\n");
	printf ("a < 0    ----> operação inválida em sqrt\n");
	printf ("a = 0    ----> divisão por zero em 1/a\n");
	printf ("Entre com o valor de a\n");

	for (;;)
	{
		if (scanf ("%Lg", &a) < 0)
			exit (0);

		printf ("a = %25.20G\n", a);
		e = exp10 (a);
		printf ("exp10 = %25.20G\n", e);
		b = sqrt (a);
		printf ("sqrt = %25.20G\n", b);
		d = 1.0 / a;
		printf ("1/a = %25.20G\n", d);
	}
}

void
fexcepid (int sig)
{
	printf ("fexcepid: interceptou o sinal -> status = %04X endereço = %P\n",
		 _fgsr (), _fgiar ());

	if ((_fgsr () & FP_INVALOPER) == FP_INVALOPER)
		printf ("fexcepid: exceção de operação inválida\n");

	if ((_fgsr () & FP_DENORMAL) == FP_DENORMAL)
		printf ("fexcepid: eexceção de operando não normalizado\n");

	if ((_fgsr () & FP_ZERODIV) == FP_ZERODIV)
		printf ("fexcepid: exceção de divisão por zero\n");

	if ((_fgsr () & FP_OVERFLOW) == FP_OVERFLOW)
		printf ("fexcepid: exceção de overflow\n");

	if ((_fgsr () & FP_UNDERFLOW) == FP_UNDERFLOW)
		printf ("fexcepid: exceção de underflow\n");

	if ((_fgsr () & FP_PRECISION) == FP_PRECISION)
		printf ("fexcepid: exceção de precisão\n");

	_fssr (0);

	signal (SIGFPE, fexcepid);
}
