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
	int		fd, ino, n;
	daddr_t		ino_bno, bno;
	char		ino_area[BL4SZ];
	char		addr_area[BL4SZ];

	if (argc == 1)
		error ("$edino <dev> <ino> ...");

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

	if (np->n_magic != T1_SBMAGIC)
		error ("Número mágico inválido (%P :: %P)", np->n_magic, T1_SBMAGIC);

	print_inode (ino, np);

	printf
	(	"ADDRs  = %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n"
		"         %d, %d, %d\n",
		np->n_addr[0],  np->n_addr[1],  np->n_addr[2],  np->n_addr[3], 
		np->n_addr[4], 	np->n_addr[5],  np->n_addr[6],  np->n_addr[7],
		np->n_addr[8],  np->n_addr[9], 	np->n_addr[10], np->n_addr[11],
		np->n_addr[12],	np->n_addr[13], np->n_addr[14], np->n_addr[15],
		np->n_addr[16],	np->n_addr[17], np->n_addr[18]
	);

	if (np->n_addr[16])
	{
		int		i;

		llseek (fd, ((loff_t)np->n_addr[16]) << BL4SHIFT, NULL, 0);

		if (read (fd, addr_area, BL4SZ) != BL4SZ)
			error ("$*Erro de leitura");

		printf ("IADDRs =");

		for (i = 0; i < 1024; i++)
		{
			if ((((daddr_t *)addr_area)[i]) != 0)
				printf (" %d ", ((daddr_t *)addr_area)[i]);
#if (0)	/*******************************************************/
			else
				break;
#endif	/*******************************************************/
		}

		printf ("\n");
	}

	if (argc < 4)
		exit (0);

	/*
	 *	Alterações
	 */
	switch (argv[3][0])
	{
	    case 'g':
	    case 'G':
		printf ("Zera o número mágico\n");
		np->n_magic = 0;
		break;

	    case 'm':
	    case 'M':
		n = strtol (argv[4], (const char **)NULL, 0);
		printf ("Novo MODE = %P\n", n);
		np->n_mode = n;
		break;

	    case 's':
	    case 'S':
		n = strtol (argv[4], (const char **)NULL, 0);
		printf ("Novo SIZE = %d\n", n);
		np->n_size = n;
		break;

	    case 'l':
	    case 'L':
		n = strtol (argv[4], (const char **)NULL, 0);
		printf ("Novo LINK = %d\n", n);
		np->n_nlink = n;
		break;

	    case 'b':
	    case 'B':
		n = strtol (argv[4], (const char **)NULL, 0);
		bno = strtol (argv[5], (const char **)NULL, 0);
		printf ("Bloco %d, novo valor = %d\n", n, bno);

#if (0)	/*******************************************************/
		if (n >= 13)
		{
			if (addr[10] == 0)
			{
				printf ("Não há bloco indireto\n");
				exit (0);
			}

			ind[n - 13] = bno;

			for (i = 0; i < 128; i++)
				ind[i] = INO_ENDIAN_LONG (ind[i]);

			write_bno = addr[10];
			write_ptr = ind;
		}
		else
#endif	/*******************************************************/
		{
			np->n_addr[n] = bno;
		}
		break;

	    default:
		printf ("Não entendi\n");
		exit (0);
	}

	printf ("Modifica? (n): ");

	if (askyesno () <= 0)
		exit (0);

	llseek (fd, (loff_t)ino_bno << BL4SHIFT, NULL, 0);

	if (write (fd, ino_area, BL4SZ) != BL4SZ)
		error ("$*Erro de escrita");

	printf ("Modifiquei ...\n");

	return (0);

}	/* end main */

void
print_inode (int ino, T1DINO *np)
{
	register char	*cp;

	printf ("\nINODE  = %d:  ", ino);

	cp = modetostr (np->n_mode); printf ("%c  %s  ", *cp, cp + 14);

	printf ("%d  ", np->n_nlink);

	if ((cp = (char *)pwcache (np->n_uid)) != NOSTR)
		printf ("%s  ", cp);
	else
		printf ("usuário %d  ", np->n_uid);

	printf ("%d\n", np->n_size);

}	/* end print_inode */
