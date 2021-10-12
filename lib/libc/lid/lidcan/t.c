
#include <sys/types.h>

main ()
{
	register char	*cp;
	char		buf[80];
	lid_t		lid;
	extern char	*gets ();
	extern idp_t	lidcan ();

	while (gets (buf))
	{
		if (lidcan (lid, buf) != lid)
			printf ("Não retornou \"lid\"\n");

		printf ("%s\n%s\n", buf, lid);
	}
}
