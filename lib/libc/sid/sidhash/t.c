#include <sys/types.h>

extern idp_t	sidcan (), sidcpy (), sidclr ();
extern char	*gets ();

main (argc, argv)
register char	**argv;
{
	sid_t	a, b;
	char	line[30];

	sidcan (b, "q");

	for (;;)
	{
		gets (line);
		sidcan (a, line);
		if (sideq (a, b))
			exit (0);
		printf
		(	"sid = \"%s\" sidhash = %d, hash = %d\n",
			a,
			sidhash (a, 1009),
			hash (a)
		); 
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
	 *	Calcula o EOR das 4 short's
	 */
	np = (ushort *)name;
	nameend = (ushort *)((char *)name + SIDSZ+1);

	xor = *np++;
	while (np < nameend)
		xor ^= *np++;

	/*
	 *	Calcula a primeira tentativa
	 */
	return (xor %= 1009);

}
