#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>
#include <errno.h>

int
main (int argc, const char *argv[])
{
	int		n;
	SYM		*sp;
	char		area[80];

	sp = alloca (SYM_SIZEOF (80));

	while (gets (area) == area)
	{
		n = strtol (area, (const char **)0, 16);

		sp->s_nm_len = 80;

		if (kcntl (K_GETSYMGA, n, sp) < 0)
			error ("*Erro para \"%s\"", area);

		printf
		(	"id = %s, len = %d, end = %P, tipo = %02X\n",
			sp->s_name, sp->s_nm_len, sp->s_value, sp->s_type
		);

	}

	return (0);
}
