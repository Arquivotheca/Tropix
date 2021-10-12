/*
 ****************************************************************
 *								*
 *			cpfs.c					*
 *								*
 *	Copia sistemas de arquivos				*
 *								*
 *	Versão	1.0.0, de 05.03.86				*
 *		4.5.0, de 21.09.03				*
 *								*
 *	Módulo: cpfs						*
 *		Utilitários de Manutenção			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/t1.h>
#include <sys/v7.h>
#include <sys/stat.h>
#include <sys/disktb.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ustat.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.5.0, de 21.09.03";

entry int	vflag;		/* Verbose */

#define	NB	128		/* No. de Blocos por leitura/escrita */

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

/*
 ******	Protótipos de funções ***********************************
 */
int		cv_blocks_to_MB (long);
int		cv_blocks_to_MB_tenths (long);
void		help (void);

/*
 ****************************************************************
 *	Copia Sistemas de arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, i;
	STAT		src_s, dst_s;
	USTAT		u;
	char		area[NB*BLSZ];
	int		narea, rest;
	int		src_fd, dst_fd;
	long		blocks;
	DISKTB		src_ud, dst_ud;
	char		src_partnm[32], dst_partnm[32];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (argc != 2)
		help ();

	/*
	 *	Examina a fonte
	 */
	if ((src_fd = open (argv[0], O_RDONLY)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	if (fstat (src_fd, &src_s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	if ((src_s.st_mode & S_IFMT) != S_IFCHR)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" não é um dispositivo de caracteres\n",
			pgname, argv[0]
		);
		exit (1);
	}

	if (ustat (src_s.st_rdev, &u) >= 0)
	{
		fprintf
		(	stderr,
			"%s: CUIDADO: O dispositivo fonte \"%s\" está montado\n",
			pgname, argv[0]
		);
	}

	if (kcntl (K_GET_DEV_TB, src_s.st_rdev, &src_ud) < 0)
	{
		printf
		(	"%s: Não consegui obter o tamanho do dispositivo \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	src_partnm[0] = '\0';
	kcntl (K_GET_PARTNM, src_ud.p_type, src_partnm);

	printf
	(	"Fonte:   \"%s\", %s, %d blocos (%d.%1d MB)\n",
		argv[0], src_partnm, 
		src_ud.p_size, cv_blocks_to_MB (src_ud.p_size),
		cv_blocks_to_MB_tenths (src_ud.p_size)
	);

	/*
	 *	Examina o destino
	 */
	if ((dst_fd = open (argv[1], O_RDWR)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, argv[1], strerror (errno)
		);
		exit (1);
	}

	if (fstat (dst_fd, &dst_s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, argv[1], strerror (errno)
		);
		exit (1);
	}

	if ((dst_s.st_mode & S_IFMT) != S_IFCHR)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" não é um dispositivo de caracteres\n",
			pgname, argv[1]
		);
		exit (1);
	}

	if (ustat (dst_s.st_rdev, &u) >= 0)
	{
		fprintf
		(	stderr,
			"%s: O dispositivo destino \"%s\" está montado\n",
			pgname, argv[1]
		);
		exit (1);
	}

	if (kcntl (K_GET_DEV_TB, dst_s.st_rdev, &dst_ud) < 0)
	{
		printf
		(	"%s: Não consegui obter o tamanho do dispositivo \"%s\" (%s)\n",
			pgname, argv[1], strerror (errno)
		);
		exit (1);
	}

	dst_partnm[0] = '\0';
	kcntl (K_GET_PARTNM, dst_ud.p_type, dst_partnm);

	printf
	(	"Destino: \"%s\", %s, %d blocos (%d.%1d MB)\n",
		argv[1], dst_partnm, 
		dst_ud.p_size, cv_blocks_to_MB (dst_ud.p_size),
		cv_blocks_to_MB_tenths (dst_ud.p_size)
	);

	/*
	 *	Calcula o No. de áreas a copiar
	 */
	blocks = MIN (src_ud.p_size, dst_ud.p_size);
	narea  = blocks / NB;
	rest   = blocks % NB;

#undef	DEBUG
#ifdef	DEBUG
	printf ("narea = %d, rest = %d de %d blocos\n", narea, rest, NB);
#endif	DEBUG

	/*
	 *	Última chance
	 */
	fprintf
	(	stderr,
		"Última chance antes de escrever em \"%s\" - "
		"continua? (n): ",
		argv[1]
	);

	if (askyesno () <= 0)
		exit (2);

	/*
	 *	Malha de Copia
	 */
	for (i = 0; i < narea; i++)
	{
		if (read (src_fd, area, sizeof (area)) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura de \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}

		/* Atualiza o nome do dispositivo no SB */

		if (i == 0)
		{
			if   (src_ud.p_type == PAR_TROPIX_T1)
			{
				T1SB		*sbp = (T1SB *)(area + BL4SZ);

				if (sbp->s_magic == T1_SBMAGIC)
					strcpy (sbp->s_fpack, dst_ud.p_name);
			}
			elif (src_ud.p_type == PAR_TROPIX_V7)
			{
				V7SB		*sbp = (V7SB *)(area + BLSZ);

				if (ENDIAN_LONG (sbp->s_magic) == V7_SBMAGIC)
					strcpy (sbp->s_fpack, dst_ud.p_name);
			}
		}

		if (write (dst_fd, area, sizeof (area)) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de escrita de \"%s\" (%s)\n",
				pgname, argv[1], strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Escreve o resto
	 */
	if (rest)
	{
		if (read (src_fd, area, rest * BLSZ) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura de \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}

		if (write (dst_fd, area, rest * BLSZ) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de escrita de \"%s\" (%s)\n",
				pgname, argv[1], strerror (errno)
			);
			exit (1);
		}
	}

	close (src_fd);
	close (dst_fd);

	return (0);

}	/* end cpfs */

/*
 ****************************************************************
 *	Converte blocos em Mb (Parte inteira)			*
 ****************************************************************
 */
int
cv_blocks_to_MB (long daddr)
{
	return ((unsigned)daddr >> (MBSHIFT-BLSHIFT));

}	/* end cv_blocks_to_MB */

/*
 ****************************************************************
 *	Converte blocos em Mb (Décimos)				*
 ****************************************************************
 */
int
cv_blocks_to_MB_tenths (long daddr)
{
	return (10 * ((unsigned)daddr & (MBSZ/BLSZ - 1))) >> (MBSHIFT-BLSHIFT);

}	/* end cv_blocks_to_MB_tenths */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - copia sistemas de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s <fonte> <destino>\n",
		pgname, pgversion, pgname
	);
	exit (2);

}	/* end help */
