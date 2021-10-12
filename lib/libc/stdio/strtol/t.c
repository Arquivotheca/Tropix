#include <stdlib.h>

main ()
{
	long		val;
	const char	*str;

	val = strtol ("  \t  1234<===", &str, 0);
	printf ("Valor = %d, próximo = \"%s\"\n", val, str);

	val = strtol ("  \t  -00001234<===", &str, 10);
	printf ("Valor = %d, próximo = \"%s\"\n", val, str);

	val = strtol ("  \t  +ABCD<===", &str, 16);
	printf ("Valor = %X, próximo = \"%s\"\n", val, str);

	val = strtol ("  \t  +01234<===", &str, 0);
	printf ("Valor = 0%O, próximo = \"%s\"\n", val, str);

	val = strtol ("  \t  +0x1234<===", &str, 0);
	printf ("Valor = 0x%X, próximo = \"%s\"\n", val, str);

}
