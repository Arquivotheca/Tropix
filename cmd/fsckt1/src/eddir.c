#include <sys/common.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void		print_inode (int ino, T1DINO *np);

int
main (int argc, const char *argv[])
{
	T1DINO		*np;
	T1DIR		*dp, *end_dp;
	int		fd, ino, n, lbn = 0;
	int		ino_bno;
	daddr_t		addr = 0;
	char		ino_area[BL4SZ];
	char		dir_area[BL4SZ];

	if (argc == 1)
		error ("$eddir <dev> <ino> <nm> ...");

	askyesno_stderr = stdout;

	if ((fd = open (argv[1], 2)) < 0)
		error ("$*Não consegui abrir \"%s\"", argv[1]);

	if ((ino = atol (argv[2])) <= 0)
		error ("$No. de INODE inválido: \"%s\"", argv[2]);

	ino_bno = T1_INOtoADDR (ino);

	llseek (fd, (loff_t)ino_bno << BL4SHIFT, NULL, 0);

	if (read (fd, ino_area, BL4SZ) != BL4SZ)
		error ("$*Erro de leitura");

	np = (T1DINO *)ino_area + T1_INOtoINDEX (ino);

	if (!S_ISDIR (np->n_mode))
		error ("$NÃO é um diretório");

	for (lbn = 0, dp = end_dp = NOT1DIR; /* abaixo */; dp = T1DIR_SZ_PTR (dp))
	{
		if (dp >= end_dp)
		{
			if ((addr = np->n_addr[lbn++]) == 0)
				error ("$NÃO Achei");

			llseek (fd, (loff_t)addr << BL4SHIFT, NULL, 0);

			if (read (fd, dir_area, BL4SZ) != BL4SZ)
				error ("$*Erro de leitura");

			dp = (T1DIR *)dir_area; end_dp = (T1DIR *)(dir_area + BL4SZ);
		}

		error ("name = %s, ino = %d", dp->d_name, dp->d_ino);

		if (streq (dp->d_name, argv[3]))
			break;
	}

	/*
	 *	Achou a entrada
	 */
	error ("Achei: ino = %d, name = \"%s\", sz = %d", dp->d_ino, dp->d_name, dp->d_ent_sz);

	if (argc < 5)
		exit (0);

	/*
	 *	Alterações
	 */
	switch (argv[4][0])
	{
	    case 'i':
	    case 'I':
		n = atoi (argv[5]);
		printf ("Novo INO = %d\n", n);
		dp->d_ino = n;
		break;

	    case 'n':
	    case 'N':
		if (argv[5])
		{
			printf ("Novo NOME = \"%s\"\n", argv[5]);

			if (T1DIR_SIZEOF (strlen (argv[5])) > dp->d_ent_sz)
				error ("Cuidado: Tamanho excessivo");

			strcpy (dp->d_name, argv[5]);
		}
		else
		{
			printf ("Novo NOME = \"(NULO)\"\n");

			dp->d_name[0] = '\0';
		}

		break;

	    case 'z':
	    case 'Z':
		printf ("Zera a entrada\n");
		n = dp->d_ent_sz;
		memset (dp, 0, n);
		dp->d_ent_sz = n;
		break;

	    case 's':
	    case 'S':
		n = atoi (argv[5]);
		printf ("Novo SZ = %d\n", n);
		dp->d_ent_sz = n;
		break;

	    default:
		printf ("Não entendi\n");
		exit (0);
	}

	printf ("Modifica? (n): ");

	if (askyesno () <= 0)
		exit (0);

	llseek (fd, (loff_t)addr << BL4SHIFT, NULL, 0);

	if (write (fd, dir_area, BL4SZ) != BL4SZ)
		error ("$*Erro de escrita");

	printf ("Modifiquei ...\n");

	return (0);

}	/* end main */
