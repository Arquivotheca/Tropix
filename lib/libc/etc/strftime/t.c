#define	SZ	132

#include <stdio.h>
#include <time.h>
#include <locale.h>

main (int argc, char *argv[])
{
	int		n, i;
	time_t		data;
	TM		*tm;
	char		buf[SZ];

	if (argv[1])
	{
		printf
		(	"setlocale LC_TIME (%s) = \"%s\"\n\n",
			argv[1], setlocale (LC_TIME, argv[1])
		);
	}

	time (&data);

	for (i = 2; i > 0; i--, data += (20 * 365 * 24 * 3600))
	{
		tm = localtime (&data);
		printf ("\n****************** %s", brasctime (tm));

		n = strftime (buf, SZ, "Linha com o dia \"%a\" da semana", tm);
		printf ("\n\n%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o dia \"%A\" da semana", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o mês \"%b\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o mês \"%B\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com a data + hora \"%c\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o dia \"%d\" do mes", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com a hora \"%H\" do dia", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com a hora \"%I\" do dia", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com o dia \"%j\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com o mes \"%m\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com o minuto \"%M\" da hora", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com o AM/PM \"%p\" da hora", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com o segundo \"%S\" do minuto", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com a semana (D) \"%U\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o dia da semana \"%w\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha de bobagens com a semana (S) \"%W\" do ano", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com a data \"%x\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com a hora \"%X\" do dia", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o ano \"%y\" sem século", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o ano \"%Y\" com século", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Linha com o Timezone \"%Z\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "Teste do '%%' e de inválidos '%E'", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, SZ, "\"%a\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, 4, "\"%a\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, 3, "\"%a\"", tm);
		printf ("%3d: \"%s\"\n", n, buf);

		n = strftime (buf, 2, "Linha de bobagens com o dia \"%a\" da semana", tm);
		printf ("%3d: \"%s\"\n\n", n, buf);
	}


	exit (0);
}
