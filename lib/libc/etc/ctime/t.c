main ()
{
	long	t;

	time (&t);

	printf ("%s", ctime (&t));
	printf ("%s", btime (&t));
}
