
#include <time.h>
#include <stdio.h>


main (int argc, register char *argv[])
{
	time_t		TIME;
	char		buf[32];

	while (gets (buf))
	{
		time (&TIME);
		TIME = strtotime (buf, localtime (&TIME));

		printf ("%s => ", buf);

		if (TIME != -1)
		{
			strftime (buf, 32, "%c", gmtime (&TIME));
			printf ("%s\n", buf);
		}
		else
		{
			printf ("ERRO\n");
		}
	}
}
