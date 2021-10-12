#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

main ()
{
	register PASSWD	*pw;
	char		area[80];

	while ((pw = getpwent ()) != NOPASSWD)
	{
		printf
		(	"%3d %-14s %6d %-10s %-10s\n",
			getpwslot (), pw->pw_name, pw->pw_uid,
			pw->pw_dir, pw->pw_shell
		);
	}

	endpwent (); printf ("\n");

	while (printf ("Usuário: "), gets (area) == area)
	{
		if ((pw = getpwnam (area)) != NOPASSWD)
		{
			printf
			(	"%3d %-14s %6d %-10s %-10s\n",
				getpwslot (), pw->pw_name, pw->pw_uid,
				pw->pw_dir, pw->pw_shell
			);
		}
		else
		{
			printf ("Usuário \"%s\" não encontrado\n", area);
		}
	}

	exit (0);

}	/* end t */
