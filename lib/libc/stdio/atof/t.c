 
char		buf[80];

double	atof ();

extd	d;

main ()
{
	register int	*ip;

	for (;;)
	{
		gets (buf);
		d = atof (buf);
		printf ("%e	%f	%g\n", d, d, d);
	}
}
