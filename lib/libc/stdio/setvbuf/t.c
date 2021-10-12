#include <stdio.h>

main (argc, argv)
char **argv;
{
	register FILE	*ifp, *ofp;
	register int	c;

	if ((ifp = fopen (argv[1], "r")) == NOFILE) 
	{
		printf ("Não consegui abrir \"%s\"\n", argv[1]);
		exit (1);
	}

	if (streq (argv[2], "-"))
	{
		ofp = stdout;
	}
	else if ((ofp = fopen (argv[2], "w")) == NOFILE) 
	{
		printf ("Não consegui abrir \"%s\"\n", argv[2]);
		exit (1);
	}

	if ((c = atoi (argv[3])) <= 0)
	{
		printf ("Tamanho inválido: \"%s\"\n", argv[3]);
		exit (1);
	}

	if (argv[4])
	{
		setvbuf (ifp, 0, _IOFBF, c);
		setvbuf (ofp, 0, _IOLBF, c);
	}
	else
	{
		setvbuf (ifp, 0, _IOFBF, c);
		setvbuf (ofp, 0, _IOFBF, c);
	}

	while ((c = getc (ifp)) != EOF)
		putc (c, ofp);

	status (ifp);
	status (ofp);
}
