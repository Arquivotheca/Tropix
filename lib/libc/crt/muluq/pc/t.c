int
main (int argc, const char *argv[])
{
	long long		x;
	unsigned long long	z;
	int			i;

	for (x = 1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", x);

		x *= 10;
	}

	for (x = -1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", x);

		x *= 10;
	}

	for (x = -1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", x);

		x *= -10;
	}

	for (z = 1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", z);

		z *= 10;
	}

	for (z = -1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", z);

		z *= 10;
	}

	for (z = -1, i = 0; i < 19; i++)
	{
		printf ("%qd\n", z);

		z *= -10;
	}

	return (0);
}
