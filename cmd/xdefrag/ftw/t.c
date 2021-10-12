#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>

#include "ftw.h"


#define		EVER	;;
#define		elif	else if

int		func (const char *, const STAT *, int);

entry unsigned long	stack;

entry int	file_count, dir_count, index;

main (int argc, register const char *argv[])
{
	int		ret;

	stack = (unsigned long)&ret;

	ret = ftw (argv[1], func);

	printf
	( 	"\nFTW acabou com %d, stack = %d bytes, file = %d, dir = %d\n",
		ret, (unsigned long)&ret - stack, file_count, dir_count
	);
}

int
func (const char *path, const STAT *sp, int cte)
{
	char		*link;
	const char	*cp;
	int		level;

	if ((unsigned long)&path < stack)
		stack = (unsigned long)&path;

	switch (cte)
	{
	    case FTW_F:
		for (level = 0, cp = path; *cp != '\0'; cp++)
		{
			if (*cp == '/')
				level++;
		}

		printf ("  %d:	%10d (%d)	%s:\n", index++, sp->st_size, level, path);
		file_count++;
		break;

	    case FTW_SL:
		printf ("SL	%s", path);

		link = alloca (sp->st_size + 1);

		if (readlink (path, link, sp->st_size + 1) < 0)
			{ error ("Não consegui obter o conteúdo do elo simbólico"); break; }

		printf (" => %s\n", link);
		break;

	    case FTW_D:
		printf ("\n****** Diretório \"%s\"\n\n", path);
		dir_count++;
		break;

	    case FTW_NS:
		printf ("??? %s\n", path);
		break;

	    case FTW_DNR:
		printf ("\n? D %s\n", path);
		break;

	    default:
		printf ("Erro na ftw, cte = %d\n", cte);
	}

	return (0);

}
