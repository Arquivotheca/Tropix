main (argc, argv)
char **argv;
{
	printf ("%s: %d\n", argv[1] , ttyslot (argv[1]) );
	exit (0);
}
