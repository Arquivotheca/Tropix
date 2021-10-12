#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <errno.h>

int
main (int argc, const char *argv[])
{
	SYM		*sp;
	char		area[80];

	sp = alloca (SYM_SIZEOF (80));

	while (gets (area) == area)
	{
		sp->s_nm_len = strlen (area); strcpy (sp->s_name, area);

		if (kcntl (K_GETSYMGN, sp->s_nm_len, sp) < 0)
			error ("*Erro para \"%s\"", area);

		printf
		(	"id = %s, end = %P, tipo = %02X\n",
			sp->s_name, sp->s_value, sp->s_type
		);
	}

	return (0);
}
