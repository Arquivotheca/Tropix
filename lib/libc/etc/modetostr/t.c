
#include <sys/types.h>
#include <stat.h>
#include <ctype.h>


main ()
{
	int		mode;
	extern char	*modetostr ();
	extern char	*gets ();
	char		line[100];
	STAT		statb;

	for (;;)
	{
		printf ("Entre modo/arquivo: ");

		if (gets (line) != line)
			exit (0);

		if (isxdigit (line[0]))
		{
			mode = strtol (line, 0, 0);
		}
		else
		{
			if (stat (line, &statb) < 0)
			{
				printf
				(	"Não consegui obter o "
					"estado de \"%s\"\n",
					line
				);
				continue;
			}
			mode = statb.st_mode;
		}

		printf
		(	"0x%X (0%o)  é \"%s\"\n",
			mode,
			mode,
			modetostr (mode)
		);
	}
}
