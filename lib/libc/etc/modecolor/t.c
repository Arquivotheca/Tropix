#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>


int
main (int argc, const char *argv[])
{
	char			*index_vector;
	const char		**string_vector;
	const char		*restore_color;
	const char		*field;
	char			area[64];


	if (argv[1] != NOSTR)
	{
		strcpy (area, "MODECOLOR=");
		strcat (area, argv[1]);
		putenv (area);
	}

	if ((field = getenv ("MODECOLOR")) != NOSTR)
		printf ("%s\n\n", field);

	if (modecolor (isatty (fileno (stdout)), &index_vector, &string_vector) < 0)
		error ("Erro de sintaxe ou Falta de memória em \"modecolor\"");

#if (0)	/*******************************************************/
	const char		*string;

	printf ("%d\n", S_IFDIR >> 12);
	printf ("%d\n", index_vector[S_IFDIR >> 12]);
	string = string_vector[S_IFDIR >> 12];
	printf ("%P, %02X\n", string, string[0]);
#endif	/*******************************************************/

	restore_color = string_vector[MC_DEF];

	printf ("DIR (%d) \t= %d\n", S_IFDIR >> 12, index_vector[S_IFDIR >> 12]);
	printf ("REG (%d) \t= %d\n", S_IFREG >> 12, index_vector[S_IFREG >> 12]);
	printf ("LNK (%d) \t= %d\n", S_IFLNK >> 12, index_vector[S_IFLNK >> 12]);
	printf ("BLK (%d) \t= %d\n", S_IFBLK >> 12, index_vector[S_IFBLK >> 12]);
	printf ("CHR (%d) \t= %d\n", S_IFCHR >> 12, index_vector[S_IFCHR >> 12]);
	printf ("IFO (%d) \t= %d\n", S_IFIFO >> 12, index_vector[S_IFIFO >> 12]);
	printf ("WRN (%d) \t= %d\n", MC_WARN,	    index_vector[MC_WARN]);
	printf ("ERR (%d) \t= %d\n", MC_ERR,	    index_vector[MC_ERR]);
	printf ("\n");
	
	printf ("=>%s*** Texto DIR ***%s<=\n", string_vector[S_IFDIR >> 12], restore_color);
	printf ("=>%s*** Texto REG ***%s<=\n", string_vector[S_IFREG >> 12], restore_color);
	printf ("=>%s*** Texto LNK ***%s<=\n", string_vector[S_IFLNK >> 12], restore_color);
	printf ("=>%s*** Texto BLK ***%s<=\n", string_vector[S_IFBLK >> 12], restore_color);
	printf ("=>%s*** Texto CHR ***%s<=\n", string_vector[S_IFCHR >> 12], restore_color);
	printf ("=>%s*** Texto IFO ***%s<=\n", string_vector[S_IFIFO >> 12], restore_color);
	printf ("=>%s*** Texto WRN ***%s<=\n", string_vector[MC_WARN],	     restore_color);
	printf ("=>%s*** Texto ERR ***%s<=\n", string_vector[MC_ERR],	     restore_color);

	return (0);
}
