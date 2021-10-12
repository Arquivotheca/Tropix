#include <sys/common.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int
main (int argc, register const char *argv[])
{
	const char	*str;
	pg_t		limit;
	char		buf[80];

	limit = kcntl (K_MMU_LIMIT, -1);

	printf
	(	"MMU_LIMIT = %d PG (%d KB)\n",
		limit, PGTOKB (limit)
	);

	while (gets (buf) == buf)
	{
		limit = strtol (buf, &str, 0);

		printf ("Vou alocar limite de %d KB: ", limit); getchar ();

		limit = PGTOKB (kcntl (K_MMU_LIMIT, KBTOPG (limit)));

		printf ("Novo limite: %d KB\n", limit);
	}

}	/* end mmulimit */
