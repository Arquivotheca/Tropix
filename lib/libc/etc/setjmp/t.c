#include <setjmp.h>

	jmp_buf	env;

	int	g = 0;

	main ()
	{
		int	 s = 0;
		register r = 0;

		if (setjmp (env) != 0)
		{
			printf ("Segundo retorno,  ");
			printf ("g = %d, s = %d, r = %d\n",
				g, s, r);
			exit (0);
		}

		printf ("Primeiro retorno, ");
		printf ("g = %d, s = %d, r = %d\n",
			g, s, r);

		g = s = r = 1;

		sub ();

	}	/* end main */

	sub ()
	{
		longjmp (env, 1);

	}	/* end sub */
