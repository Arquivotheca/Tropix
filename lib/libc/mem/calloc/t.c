main ()
{
	extern char	*calloc ();
	extern char	*malloc ();
	register char	*a;

	a = malloc (1024);

	a[0]    = 55;
	a[1023] = 77;

	printf
	(	"malloc: &a = %X,  a[0] = %d, a[1023] = %d\n", 
		a,
		a[0],
		a[1023]
	);

	free (a);

	a = calloc (1024, 1);

	printf
	(	"calloc: &a = %X,  a[0] = %d, a[1023] = %d\n", 
		a,
		a[0],
		a[1023]
	);

	free (a);

}
