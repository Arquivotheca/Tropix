#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <init.h>

int
main (int argc, const char *argv[])
{
	INITAB		*initp;
	char		area[80];

	while ((initp = getinitent ()) != NOINITAB)
	{
		printf
		(	"%3d: \"%c\" %-14s %-14s %-14s\n",
			initp->it_slot, initp->it_state, initp->it_name,
			initp->it_term, initp->it_local
		);
	}

	endinitent (); printf ("\n");

	while (printf ("Linha: "), gets (area) == area)
	{
		if ((initp = getinitnam (area)) != NOINITAB)
		{
			printf
			(	"%3d: \"%c\" %-14s %-14s %-14s\n",
				initp->it_slot, initp->it_state, initp->it_name,
				initp->it_term, initp->it_local
			);
		}
		else
		{
			printf ("Usuário \"%s\" não encontrado\n", area);
		}
	}

	return (0);

}	/* end t */
