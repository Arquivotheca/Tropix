 

extern int	atoi ();
extern char	*gets ();

main ()
{
	char		buf[80];
	register int	i;

	while (gets (buf) == buf)
	{
		i = atoi (buf);
		printf ("%d\n", i);
	}
}
