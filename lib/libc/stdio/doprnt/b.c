#include <stdio.h>
#include <stdlib.h>

const char	status_bits[] =
				"\x20" "BIT_0x80000000"
				"\x11" "BIT_0x00010000"
				"\x10" "BIT_0x8000"
				"\x09" "BIT_0x0100"
				"\x08" "BIT_0x80"
				"\x07" "BIT_0x40"
				"\x06" "BIT_0x20"
				"\x05" "BIT_0x10"
				"\x04" "BIT_0x08"
				"\x03" "BIT_0x04"
				"\x02" "BIT_0x02"
				"\x01" "BIT_0x01";
print (int i)
{
		printf ("%%b		\"%b\"\n", i, status_bits);
		printf ("%%B		\"%B\"\n", i, status_bits);
		printf ("%%.2B		\"%.2B\"\n", i, status_bits);
		printf ("%%.4B		\"%.4B\"\n", i, status_bits);
		printf ("%%.8B		\"%.8B\"\n", i, status_bits);
		printf ("%%#b		\"%#b\"\n", i, status_bits);
		printf ("%%#.2b		\"%#.2b\"\n", i, status_bits);
		printf ("%%#.4b		\"%#.4b\"\n", i, status_bits);
		printf ("%%#.8b		\"%#.8b\"\n", i, status_bits);
		printf ("%%#B		\"%#B\"\n", i, status_bits);
		printf ("%%hB		\"%hB\"\n", i, status_bits);
		printf ("%%+B		\"%+B\"\n", i, status_bits);
		printf ("%%-B		\"%-B\"\n", i, status_bits);
		printf ("%% B		\"% B\"\n", i, status_bits);
		printf ("%%15B		\"%15B\"\n", i, status_bits);
		printf ("%%015B		\"%015B\"\n", i, status_bits);
		printf ("%%15.0B		\"%15B\"\n", i, status_bits);
		printf ("%%15.3B		\"%15.3B\"\n", i, status_bits);
		printf ("%%015.3B		\"%015.3B\"\n", i, status_bits);
		printf ("%%-15.3B		\"%-15.3B\"\n", i, status_bits);
		printf ("%%-015.3B	\"%-015.3B\"\n", i, status_bits);
		printf ("%%+15.3B		\"%+15.3B\"\n", i, status_bits);
		printf ("%%+015.3B	\"%+015.3B\"\n", i, status_bits);
		printf ("%%-+15.3B	\"%-+15.3B\"\n", i, status_bits);
		printf ("%%-+015.3B	\"%-+015.3B\"\n", i, status_bits);
		printf ("%% 15.3B		\"% 15.3B\"\n", i, status_bits);
		printf ("%% 015.3B	\"% 015.3B\"\n", i, status_bits);
}

main ()
{
	printf ("Valor = 0x00\n");
	print (0x00);

	printf ("\nValor = 0x01\n");
	print (0x01);

	printf ("\nValor = 0x80\n");
	print (0x80);

	printf ("\nValor = 0x35\n");
	print (0x35);

	printf ("\nValor = 0x80018100\n");
	print (0x80018100);
}

