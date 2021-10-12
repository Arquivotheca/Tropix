/*
 ****************************************************************
 *	Programa de Teste					*
 ****************************************************************
 */
main()
{
	char	original[256];
	char	line[256];

	strcpy (original,
		"Por	mais	que	voce tente, jamais editara' esta linha");
	edit ("Edit: ", original, line);

	printf ("Nova: %s$\n", line);

	exit (0);
}
