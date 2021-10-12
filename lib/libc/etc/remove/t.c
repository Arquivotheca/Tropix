
main (int argc, register char *argv[])
{
	if (remove (argv[1]) != 0)
		perror (argv[1]);
}
