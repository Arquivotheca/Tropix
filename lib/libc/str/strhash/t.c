#include <stdio.h>

main (int argc, const char *argv[])
{
	char		line[120];
	lid_t		id;
	int		i, lid_hash, str_hash;

	for (;;)
	{
		if (gets (line) != line)
			break;

		lidcan (id, line);

		printf
		(	"id = \"%s\" strhash = %d, lidhash = %d\n", 
			line,
			strhash (line, strlen (line), 1009),
			lidhash (id, 1009)
		); 
	}

	/*
	 *	x
	 */
	for (i = 0; i < 31; i++)
	{
		line[i] = 'a' + i; line[i+1] = '\0';
#if (0)	/*******************************************************/
		line[i] = '*'; line[i+1] = '\0';
#endif	/*******************************************************/

		str_hash = strhash (line, strlen (line), 1009);
		lid_hash = lidhash (lidcan (id, line), 1009);

		printf
		(	"%2d: %s, strhash = %3d, lidhash = %3d, line = \"%s\"\n", 
			i + 1,
			str_hash == lid_hash ? " OK" : "***",
			str_hash, lid_hash,
			line
		); 
	}

	return (0);
}
