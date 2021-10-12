main ()
{
	long long	a, b, c;

	a = 6ll; b = 5ll;
	printf ("6 - 5 = %qd\n", a - b);

	a = 5ll; b = 5ll;
	printf ("5 - 5 = %qd\n", a - b);

	a = 5ll; b = 6ll;
	printf ("5 - 6 = %qd\n", a - b);

	a = 123456789123456790ll; b = 123456789123456789ll;
	printf ("%qd\n", a - b);

	a = 123456789123456789ll; b = 123456789123456789ll;
	printf ("%qd\n", a - b);

	a = 123456789123456788ll; b = 123456789123456789ll;
	printf ("%qd\n", a - b);
}
