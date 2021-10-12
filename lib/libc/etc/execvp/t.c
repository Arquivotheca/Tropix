#include <stdio.h>
#include <stdlib.h>

int
main ()
{
	auto char	buf[100];

	while (gets (buf) == buf)
	{
		execlp (buf, buf, (char *)0);
		error ("*Erro em execlp");
	}

	error ("EOF");
	return (0);
}
