
#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>

#define	BLKMEM	4096
#define	READ	0

char	*pgname;
char	*sterr[] =
	{
		"Nao consegui dar stat em %s\n",
		"Nao consegui memoria suficiente: %d bytes\n",
		"Nao consegui abrir o arquivo %s\n",
		"Nao consegui memoria suficiente\n"
	};
		

main (argc, argv)
int	argc;
char	*argv[];
{
	char		**v;
	char		*a;
	char		*nomearq;
	STAT		*statp;
	FILE		*fp;
	register long	i, j, k;
	long		nblocos, nbytes;
	register int	c, fd;
	int		compara (), stat ();
	void		erro ();
	char		*malloc (), *realloc ();

	pgname = argv[0];
	nbytes = 0;

	if (argc > 1)
	{
		nomearq = argv[1];
		if (stat (nomearq, statp) < 0)
			erro (0, nomearq);
		nbytes = statp->st_size;
	}

	if (nbytes > 0)
	{
		if ((a = malloc (nbytes)) == (char *)0)
			erro (1, nbytes);

		if ((fd = open (nomearq, READ)) < 0)
			erro (2, nomearq);

		read (fd, a, nbytes);
	}
	else
	{
		if ((a = malloc (BLKMEM)) == (char *)0)
			erro (1);
		nblocos = 1;

		a[0] = getchar ();

		i = 1;
		j = 1;
		k = 1;

		while ((c = getchar ()) != EOF)
		{
			if (nblocos != 0 && k == BLKMEM)
			{
				nblocos++;
				if ((a = realloc (a, nblocos * BLKMEM)) ==
						(char *)0)
					erro (1, nblocos * BLKMEM);

				k = 0;
			}
	
			a[i] = c;
			i++;
			k++;
		}
	
		nbytes = i;
	}
	
	if ((v = (char **)malloc (BLKMEM)) == (char **)0)
		erro (3);

	nblocos = 1;
	v[0] = &a[0];
	j = 1;
	k = 1;

	for (i = 0; i < nbytes; i++)
	{
		c = a[i];

		if (c == '\n')
		{
			if (k == BLKMEM >> 2)
			{
				nblocos++;
				v = (char **)realloc (v, nblocos * BLKMEM); 
				if (v == (char **)0)
					erro (3);
				k = 0;
			}
			a[i] = '\0';
			v[j] = &a[i+1];
			j++;
			k++;

		}
	}

	j--;

	fprintf (stderr, "No. de elementos = %d\n", j);

	qtsort (v, j, compara);

	for (i = 0; i < j; i++)
		puts(v[i]);
}

void
erro (n, v1, v2)
int	n, v1, v2;
{
	fprintf (stderr, sterr[n], v1, v2);
	perror (pgname);
	exit (1);
}
