#include <sys/types.h>
#include <stdio.h>
#include <grp.h>

main ()
{
	register GROUP	*gr;

	while ((gr = getgrent ()) != NOGROUP)
	{
		printf ("%-14s %6d\n", gr->gr_name,
			gr->gr_gid);
	}

}	/* end t */
