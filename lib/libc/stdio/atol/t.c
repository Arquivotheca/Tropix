 

extern int	atol ();
extern char	*gets ();

main ()
{
	char		buf[80];
	register int	i;

	while (gets (buf) == buf)
	{
		i = atol (buf);
		printf ("%d\n", i);
	}
}
