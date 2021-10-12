char	*mktemp ();

char	buf[20];

main ()
{
	register	i;
	register char	*cp;

	for (i = 0; i < 30; i++)
	{
		strcpy (buf, "/tmp/teste.XXXXXX2345234");

		if ((cp = mktemp (buf)) != (char *)0)
		{
			creat (cp, 0644);
			printf ("Consegui: %s\n", cp);
		}
		else
		{
			printf ("Nao consegui: %s\n", cp);
		}
	}
}
