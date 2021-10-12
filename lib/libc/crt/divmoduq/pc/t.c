
void		printn (unsigned long long value);

int
main (int argc, const char *argv[])
{
	printn (1234567891234567ll);
	printn (12345678912345678ll);
	printn (123456789123456789ll);
	printn (1234567891234567891ll);
	printn (12345678912345678912ll);

	return (0);
}

/*
 ****************************************************************
 *	Imprime um Inteiro 					*
 ****************************************************************
 */
void
printn (unsigned long long value)
{
	char			*cp;
	char			area[64];
	long			*p;

	/*
	 *	Converte o valor para a base dada
	 */
	cp = area + 64; *--cp = '\0';

	p = (long *)&value;

	for (/* acima */; cp > area; /* abaixo */)
	{
		*--cp = '0' + divmoduq (&value, 10);

		if (p[0] == 0 && p[1] == 0)
			break;
	}

	printf ("%s\n", cp);

}	/* end printn */
