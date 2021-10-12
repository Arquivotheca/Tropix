#include <stdio.h>
#include <id.h>

main (argc, argv)
register char	**argv;
{
	id_t	a, b;
	char	line[30];

	idcan (b, "q");

	for (;;)
	{
		if (gets (line) != line)
			exit (0);

		idcan (a, line);
		if (ideq (a, b))
			exit (0);
		printf
		(	"id = \"%s\" idhash = %d, hash = %d\n",
			a,
			idhash (a, 1009),
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
	register unsigned short		xor;
	register unsigned short		*np, *nameend;

	/*
	 *	Calcula o EOR das IDSZ/4 short's
	 */
	np = (unsigned short *)name;
	nameend = (unsigned short *)((char *)name + IDSZ);

	xor = *np++;

	while (np < nameend)
		xor ^= *np++;

	/*
	 *	Calcula a primeira tentativa
	 */
	return (xor %= 1009);

}
