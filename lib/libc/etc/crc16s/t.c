main ()
{
	char	buf [256];
	int	k, s, f;
	char 	*gets ();

	k = 0;

	while (gets (buf) == buf)
	{
		k++;

		if (
			crc16 (buf, strlen (buf)) !=
			crc16s (buf, strlen (buf)) )
		{
			printf ("Diferenca na linha %d\n", k);
		}

		if (
			crc1641 (buf, strlen (buf)) !=
			crc1641s (buf, strlen (buf)) )
		{
			printf ("Diferenca (41) na linha %d\n", k);
		}
	}

	printf ("Fim. k = %d\n", k);
}
