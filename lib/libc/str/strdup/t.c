char	buf[56];

main ()
{
	gets (buf);
	printf ("%s\n", strdup (strdup (strdup (buf))) );
}
