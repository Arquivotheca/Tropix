main ()
{
	char	buf [256];
	int	k, n;
	char 	*gets ();

	gets (buf);
	n = strlen (buf);

	clockstamp ();
	for (k = 0; k < 10000; k++)
	{
			crc16 (buf, n);
	}
	printf ("Rapido = %s\n", clocktostr () );

	clockstamp ();
	for (k = 0; k < 10000; k++)
	{
			crc16s (buf, n);
	}
	printf ("Lento = %s\n", clocktostr () );

}
