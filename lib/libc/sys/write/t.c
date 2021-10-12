#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

main ()
{
	char	buf[13];

	printf ("Repare que printf usa o WRITE\n");

	if (write (18, buf, 13) < 0)
		printf ("Erro: %s\n", strerror (errno));
}
