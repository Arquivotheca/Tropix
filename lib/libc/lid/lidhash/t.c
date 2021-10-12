#include <sys/types.h>

extern idp_t	lidcan (), lidcpy (), lidclr ();
extern char	*gets ();

static char	cadeia[] = "a01234567890123456789012345678901234567890";

main (argc, argv)
register char	**argv;
{
	lid_t		a, b;
	char		line[80];
	register int	i, h, lh;

	lidcan (b, "q");

	/*
	 *	Teste automático
	 */
	if (argv[1] && streq (argv[1], "auto"))
	{
		for (i = 1; i < 40; i++)
		{
			strcpy (line, cadeia);
			line[i] = '\0';
			lidcan (a, line);
			lh =  lidhash (a, 1009),
			h = hash (a),

			printf
			(	"lidhash = %4d, hash = %4d, %s "
				"sz = %2d, \"%s\"\n",
				lh,
				h,
				h == lh ? "OK," : "**,",
				strlen (line),
				a
			); 
		}
		exit (0);
	}

	/*
	 *	Teste manual
	 */
	for (;;)
	{
		if (argv[1])
		{
			lidcan (a, argv[1]);
			i = strlen (argv[1]);
		}
		else
		{
			gets (line);
			lidcan (a, line);
			i = strlen (line);
		}

		if (lideq (a, b))
			exit (0);
		lh =  lidhash (a, 1009),
		h = hash (a),
		printf
		(	"lidhash = %4d, hash = %4d, %s "
			"sz = %2d, \"%s\"\n",
			lh,
			h,
			h == lh ? "OK," : "**,",
			i,
			a
		); 
		if (argv[1])
			break;
	}
}

/*
 ********************************************************
 *	Funcao de Hash para a Tabela de Simbolos	*
 ********************************************************
 */
int
hash (name)
idp_t	name;
{
	register ushort		xor;
	register ushort		*np, *nameend;

	/*
	 *	Calcula o EOR das LIDSZ/2 short's
	 */
	np = (ushort *)name;
	nameend = (ushort *)((char *)name + LIDSZ + 1);

	xor = *np++;
	while (np < nameend)
		xor ^= *np++;

	/*
	 *	Calcula a primeira tentativa
	 */
	return (xor %= 1009);
}
