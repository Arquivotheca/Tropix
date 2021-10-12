main ()
{
	register long	a, n, k;
	long	getrand ();

	setrand (314159269, 453806245, 13);

	a = 1234;
	n = 0;
	k = 0;

	for (;;)
	{
#define	PRINT_NOs
#ifdef	PRINT_NOs
		printf ("n = %P, %u\n", a, a);
#endif	PRINT_NOs

		n++;
		k++;

		if ((a = getrand (a)) == 1234)
			break;

		if (k == 1000000)
		{
			k = 0;
			printf("passaram-se %d numeros sem repetição \n", n);
		}

	}
	printf ("deu igual depois de %d numeros \n", n);
}
