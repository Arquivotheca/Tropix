extern int	errno;

main (argc, argv)
char	**argv;
{
	errno = atoi (argv[1]);

	perror (argv[0]);
}
