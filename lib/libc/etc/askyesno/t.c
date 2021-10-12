#include <stdio.h>

main (int argc, const char *argv[])
{
	int		i, r;

	/*
	 *	x
	 */
	fprintf (stderr, "Fase 1: Default\n\n");

	for (i = 5; i > 0; i--)
	{
		fprintf (stderr, "Pergunta? (s): ");

		r = askyesno ();

		fprintf (stderr, "Resposta = %d\n", r);
	}

	/*
	 *	x
	 */
	fprintf (stderr, "\nFase 2: set (1)\n\n");

	askyesno_set (1);

	for (i = 5; i > 0; i--)
	{
		fprintf (stderr, "Pergunta? (s): ");

		r = askyesno ();

		fprintf (stderr, "Resposta = %d\n", r);
	}

	/*
	 *	x
	 */
	fprintf (stderr, "\nFase 3: set (1) novamente\n\n");

	askyesno_set (1);

	for (i = 5; i > 0; i--)
	{
		fprintf (stderr, "Pergunta? (s): ");

		r = askyesno ();

		fprintf (stderr, "Resposta = %d\n", r);
	}

	/*
	 *	x
	 */
	fprintf (stderr, "\nFase 4: set (0)\n\n");

	askyesno_set (0);

	for (i = 5; i > 0; i--)
	{
		fprintf (stderr, "Pergunta? (s): ");

		r = askyesno ();

		fprintf (stderr, "Resposta = %d\n", r);
	}

}	/* end t */
