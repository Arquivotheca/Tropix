main (argc, argv)
char 	**argv;
{
	register void	*p, *sp;
	int		local;
	extern void	*alloca (), *getsp ();

	printf ("&local    = %P (%d)\n", &local, &local);

	sp = getsp ();

	printf ("sp antes  = %P (%d)\n", sp, sp);

	if (argv[1])
		p = alloca (atoi (argv[1]));
	else
		p = alloca (0);

	sp = getsp ();

	printf ("sp depois = %P (%d)\n", sp, sp);

	printf ("área      = %P (%d)\n", p, p);

	sleep (2);

	exit (0);
}
