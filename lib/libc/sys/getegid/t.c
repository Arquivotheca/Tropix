
main ()
{
	setgid (35);

	printf ("getegid = %d\n", getegid () );
}
