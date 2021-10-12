/*
 ****************************************************************
 *								*
 *			edboot.c				*
 *								*
 *	Gerencia "boot0" e "boot2"				*
 *								*
 *	Versão	3.0.0, de 26.07.96				*
 *		4.9.0, de 29.08.06				*
 *								*
 *	Módulo: edboot						*
 *		Utilitários de administração do sistema		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/a.out.h>
#include <sys/disktb.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <nlist.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 29.08.06";

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */
entry int	fflag;		/* Força */

/*
 ******	Nomes dos arquivos **************************************
 */
const char	mbr_nm[]	= "/etc/boot/mbr";
const char	boot0_nm[]	= "/etc/boot/h.boot0";
const char	boot1_t1_G_nm[]	= "/etc/boot/t1.g.boot1";
const char	boot1_t1_L_nm[]	= "/etc/boot/t1.l.boot1";
const char	boot1_v7_G_nm[]	= "/etc/boot/v7.g.boot1";
const char	boot1_v7_L_nm[]	= "/etc/boot/v7.l.boot1";

#define VERSION_OFFSET	8	/* strlen ("Versao: ") */

/*
 ******	Definição da tabela de partições ************************
 */
#define	PARTITION_SIZE		(4 * 16)
#define PARTITION_OFFSET	(BLSZ - 2 - PARTITION_SIZE)

/*
 ****** Protótipos de funções ***********************************
 */
void		ed_boot0 (const char *, const char *);
void		ed_boot1 (const char *, int);
void		ed_boot2 (const char *, const char *, const char *, const char *);
void		pr_boot2 (const char *, int, HEADER *, NLIST *);
void		install_boot0 (const char *);
void		do_save_mbr (const char *);
void		help (void);

/*
 ****************************************************************
 *	Gerencia "boot0" e "boot2"				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd;
	int		boot_code = -1;
	int		rwflag = 0;
	const char	*index_str = NOSTR;
	const char	*dev_str   = NOSTR;
	const char	*pgm_str   = NOSTR;
	const char	*video_str = NOSTR;
	const char	*file_nm;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "012wi:d:p:V:fvH")) != EOF)
	{
		switch (opt)
		{
		    case '0':			/* Operação em "boot0" */
		    case '1':			/* Operação em "boot1" */
		    case '2':			/* Operação em "boot2" */
			if (boot_code >= 0)
				help ();

			boot_code = opt - '0';
			break;

		    case 'w':			/* Instalação */
			rwflag = opt;
			break;

		    case 'i':			/* Índice do "boot0" */
			index_str = optarg;
			break;

		    case 'd':			/* Dispositivo do "boot2" */
			dev_str = optarg;
			break;

		    case 'p':			/* Programa do "boot2" */
			pgm_str = optarg;
			break;

		    case 'V':			/* Modo de Vídeo */
			video_str = optarg;
			break;

		    case 'f':			/* Força */
			fflag++;
			break;

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

	file_nm = *argv;

	askyesno_stderr = stdout;

	/*
	 *	Analisa a função
	 */
	switch (boot_code)
	{
		/*
		 ******	Boot0 ***********************************
		 */
	    case 0:
		if (dev_str || pgm_str || video_str)
			help ();

		if (index_str && rwflag)
			help ();

		if (file_nm == NOSTR)
		{
			if ((fd = open ((file_nm = "/dev/hda"), O_RDONLY)) >= 0)
				close (fd);
			else
				file_nm = NOSTR;
		}

		if (file_nm == NOSTR)
		{
			if ((fd = open ((file_nm = "/dev/sda"), O_RDONLY)) >= 0)
				close (fd);
			else
				file_nm = NOSTR;
		}

		if (file_nm == NOSTR)
			error ("$*Não encontrei \"/dev/hda\" nem \"/dev/sda\"");

		if   (rwflag == 'w')
			install_boot0 (file_nm);
		else
			ed_boot0 (file_nm, index_str);

		break;

		/*
		 ******	Boot1 ***********************************
		 */
	    case 1:
		if (dev_str || pgm_str || video_str)
			help ();

		if (index_str)
			help ();

		ed_boot1 (file_nm, rwflag);
		break;

		/*
		 ******	Boot2 ***********************************
		 */
	    case 2:
	    default:
		if (index_str)
			help ();

		if (file_nm == NOSTR)
			file_nm = "/boot";

		ed_boot2 (file_nm, dev_str, pgm_str, video_str);
		break;

	}	/* end switch */

	return (0);

}	/* end edboot */

/*
 ****************************************************************
 *	Processa o "boot0"					*
 ****************************************************************
 */
void
ed_boot0 (const char *file_nm, const char *index_str)
{
	char		*cp;
	const char	*str;
	int		fd, index;
	char		found = 0;
	char		*boot0_ptr = NOSTR, *file_ptr = NOSTR;
	char		*boot0_ver = "?", *file_ver = "?";
	STAT		s;
	typedef struct { char signature[13], std_boot, index; } CONTROL;

	/*
	 *	Tenta ler a última versão de "boot0"
	 */
	if   ((fd = open (boot0_nm, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
	{
		error ("*Não consegui abrir \"%s\"", boot0_nm);
	}
	elif (!S_ISREG (s.st_mode))
	{
		error ("*O arquivo \"%s\" NÃO é regular", boot0_nm);
	}
	elif (boot0_ptr = alloca (s.st_size), read (fd, boot0_ptr, s.st_size) < 0)
	{
		error ("*Não consegui ler \"%s\"", boot0_nm);
		boot0_ptr = NOVOID;
	}
	else for (cp = boot0_ptr; cp < boot0_ptr + s.st_size; cp++)
	{
		if ((cp = memchr (cp, 'V', s.st_size)) == NOSTR)
			break;

		if (memeq (cp, "Versao", sizeof ("Versao") - 1))
		{
			boot0_ver = alloca (strlen (cp) + 1);

			strcpy (boot0_ver, cp + VERSION_OFFSET);

			if ((cp = strchr (boot0_ver, '\n')) != NOSTR)
				cp[0] = '\0';

			break;
		}
	}

	close (fd);

	/* Le pelo menos 2 blocos da versão atual */

	if (boot0_ptr == NOVOID)
		s.st_size = 2 * BLSZ;

	/*
	 *	Tenta ler a versão atual de "boot0"
	 */
    again:
	if   ((fd = open (file_nm, index_str ? O_RDWR : O_RDONLY)) < 0)
	{
		error ("$*Não consegui abrir \"%s\"", file_nm);
	}
	elif (file_ptr = alloca (s.st_size), read (fd, file_ptr, s.st_size) < 0)
	{
		error ("$*Não consegui ler \"%s\"", file_nm);
	}

	/* Apaga a tabela de partições */

	memset (file_ptr + PARTITION_OFFSET, 0, PARTITION_SIZE);

	/* Apaga os 6 bytes que o Windows XP "suja" */

	memset (file_ptr + PARTITION_OFFSET - 6, 0, 6);

	/*
	 *	Verifica se tem a assinatura "55AA"
	 */
	if (*(ushort *)&file_ptr[PARTITION_OFFSET + PARTITION_SIZE] != 0xAA55)
		error ("$O arquivo \"%s\" NÃO contém a assinatura \"55AA\"", file_nm);

	/*
	 *	Verifica se tem a assinatura "TROPIX boot0"
	 */
	for (cp = file_ptr; cp < file_ptr + s.st_size; cp++)
	{
		if ((cp = memchr (cp, 'T', s.st_size)) == NOSTR)
			break;

		if (memeq (cp, "TROPIX boot0", sizeof ("TROPIX boot0") - 1))
			{ found = 1; break; }
	}

	if (!found)
		error ("$O arquivo \"%s\" NÃO contém a assinatura de \"boot0\" (0)", file_nm);

	/*
	 *	Verifica se tem a assinatura "TROPIX BOOT0"
	 */
	if (!streq (file_ptr + 2, "TROPIX BOOT0") || !streq (file_ptr + BLSZ, "TROPIX BOOT0"))
		error ("$O arquivo \"%s\" NÃO contém a assinatura de \"boot0\" (1)", file_nm);

	/*
	 *	Tenta ler a versão atual de "file"
	 */
	for (cp = file_ptr; cp < file_ptr + s.st_size; cp++)
	{
		if ((cp = memchr (cp, 'V', s.st_size)) == NOSTR)
			break;

		if (memeq (cp, "Versao", sizeof ("Versao") - 1))
		{
			file_ver = alloca (strlen (cp) + 1);

			strcpy (file_ver, cp + VERSION_OFFSET);

			if ((cp = strchr (file_ver, '\n')) != NOSTR)
				cp[0] = '\0';

			break;
		}
	}

	/*
	 *	Compara as versões
	 */
	if (boot0_ptr != NOSTR)
	{
		((CONTROL *)(boot0_ptr + BLSZ))->index = ((CONTROL *)(file_ptr + BLSZ))->index;

		if   (!memeq (file_ptr, boot0_ptr, s.st_size))
		{
			error ("A versão de \"boot0\" instalada NÃO é a atual");

			printf ("Versão instalada = \"%s\"\n", file_ver);

			printf ("Versão atual     = \"%s\"\n", boot0_ver);
		}
		elif (vflag)
		{
			error
			(	"A versão de \"boot0\" instalada (%s) É a atual",
				boot0_ver
			);
		}
	}

	/*
	 *	Apenas le o valor do índice
	 */
	if (index_str == NOSTR)
	{
		printf
		(	"Dispositivo \"%s\" (boot0): "
			"Índice da partição \"default\" = %d\n",
			file_nm, ((CONTROL *)(file_ptr + BLSZ))->index
		);
		exit (0);
	}

	/*
	 *	Retira o índice do "boot0"
	 */
	if ((index = strtol (index_str, &str, 0)) < 0 || *str != '\0')
		error ("$Índice \"%s\" inválido", index_str);

	/*
	 *	Modifica o valor do índice
	 *
	 *	Repare que só modificamos o primeiro bloco
	 */
	((CONTROL *)(file_ptr + BLSZ))->index = index;

	lseek (fd, 1 * BLSZ, SEEK_SET);

	if (write (fd, file_ptr + BLSZ, BLSZ) != BLSZ)
		error ("$*Não consegui escrever em \"%s\"", file_nm);

	close (fd); index_str = NOSTR; boot0_ptr = NOSTR;

	goto again;

}	/* end ed_boot0 */

/*
 ****************************************************************
 *	Processa o "boot1"					*
 ****************************************************************
 */
void
ed_boot1 (const char *file_nm, int rwflag)
{
	int		n, fd;
	char		L_or_G = '?';
	char		file_area[BL4SZ], boot1_area[BL4SZ];
	char		*boot1_ver = "?", *file_ver = "?";
	char		*par_code = "??";
	char		*cp;
	const char	*boot1_nm = NOSTR;
	int		boot1_sz;
	const DISKTB	*disktb, *dp;
	STAT		s;

	/*
	 *	Obtém a tabela de disco
	 */
	if ((n = kcntl (K_GET_DEV_TB, NODEV, NODISKTB)) < 0)
		error ("$*Não consegui obter o tamanho da tabela de discos");

	disktb = alloca (n * sizeof (DISKTB));

	if (kcntl (K_GET_DEV_TB, NODEV, disktb) < 0)
		error ("$*Não consegui obter a tabela de discos");

	/*
	 *	Processa o dispositivo e lê o "boot1" instalado
	 */
	if (file_nm == NOSTR)
		file_nm = "/dev/root";

	if ((fd = open (file_nm, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	if (!S_ISBLK (s.st_mode) && !S_ISCHR (s.st_mode))
		error ("$O arquivo \"%s\" NÃO é especial", file_nm);

	if (read (fd, file_area, sizeof (file_area)) != sizeof (file_area))
		error ("$*Erro na leitura de \"%s\"", file_nm);

	close (fd);

	/*
	 *	Tenta obter a versão do "boot1" instalado
	 */
	for (cp = file_area; cp < file_area + sizeof (file_area); cp++)
	{
		if ((cp = memchr (cp, 'V', sizeof (file_area))) == NOSTR)
			break;

		if (memeq (cp, "Versao", sizeof ("Versao") - 1))
		{
			file_ver = alloca (strlen (cp) + 1);

			strcpy (file_ver, cp + VERSION_OFFSET);

			if ((cp = strchr (file_ver, '\n')) != NOSTR)
				cp[0] = '\0';

			break;
		}
	}

	/*
	 *	Procura o dispositivo na tabela
	 */
	for (dp = disktb; /* abaixo */; dp++)
	{
		if (dp->p_name[0] == '\0')
			error ("$Não encontrei \"%s\" na tabela de discos", file_nm);

		if (dp->p_name[0] == '*')
			continue;

		if (dp->p_dev == s.st_rdev)
		{
			L_or_G = dp->p_head ? (dp->p_flags & DISK_INT13_EXT ? 'L' : 'G') : '?';
#undef	DEBUG
#ifdef	DEBUG
			printf ("%s, %s (%02X, %c)\n", file_nm, dp->p_name, dp->p_type, L_or_G);
#endif	DEBUG
			break;
		}
	}

	/*
	 *	Lê o boot1 atual
	 */
	if   (dp->p_type == 0xA8)
		{ par_code = "V7"; boot1_nm = (L_or_G == 'L') ? boot1_v7_L_nm : boot1_v7_G_nm; }
	elif (dp->p_type == 0xA9)
		{ par_code = "T1"; boot1_nm = (L_or_G == 'L') ? boot1_t1_L_nm : boot1_t1_G_nm; }
	else
		error ("$A partição não é \"T1\" nem \"v7\"");

	if ((fd = open (boot1_nm, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
		error ("$*Não consegui abrir \"%s\"", boot1_nm);

	boot1_sz = s.st_size;

	memset (boot1_area, 0, sizeof (boot1_area));

	if (read (fd, boot1_area, boot1_sz) != boot1_sz)
		error ("$*Erro na leitura de \"%s\"", boot1_nm);

	close (fd);

	/*
	 *	Tenta obter a versão do "boot1" atual
	 */
	for (cp = boot1_area; cp < boot1_area + boot1_sz; cp++)
	{
		if ((cp = memchr (cp, 'V', boot1_sz)) == NOSTR)
			break;

		if (memeq (cp, "Versao", sizeof ("Versao") - 1))
		{
			boot1_ver = alloca (strlen (cp) + 1);

			strcpy (boot1_ver, cp + VERSION_OFFSET);

			if ((cp = strchr (boot1_ver, '\n')) != NOSTR)
				cp[0] = '\0';

			break;
		}
	}

	/*
	 *	Compara
	 */
	if (memeq (file_area, boot1_area, boot1_sz))
	{
		if (vflag) error
		(	"A versão de \"boot1\" instalada (%s, %s, %c) É a atual",
			boot1_ver, par_code, L_or_G
		);

		exit (0);
	}

	error ("A versão de \"boot1\" instalada NÃO é a atual");

	printf ("Versão instalada = \"%s\"\n", file_ver);

	printf ("Versão atual     = \"%s\" (%s, %c)\n", boot1_ver, par_code, L_or_G);

	if (rwflag != 'w')
		exit (0);

	printf ("Instala a versão (%s, %c) atual? (n): ", par_code, L_or_G);

	if (!fflag)
	{
		if (askyesno () <= 0)
			exit (0);
	}
	else
	{
		printf ("SIM\n");
	}

	/*
	 *	Grava a versão atual
	 */
	if ((fd = open (file_nm, O_RDWR)) < 0)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	boot1_sz += BLMASK; boot1_sz &= ~BLMASK;

	if (write (fd, boot1_area, boot1_sz) !=  boot1_sz)
		error ("$*Erro na escrita de \"%s\"", boot1_nm);

	close (fd);

	sync ();

	printf ("O programa \"boot1\" foi instalado\n");

	exit (0);

}	/* end ed_boot1 */

/*
 ****************************************************************
 *	Processa o "boot2"					*
 ****************************************************************
 */
void
ed_boot2 (const char *file_nm, const char *dev_str, const char *pgm_str, const char *video_str)
{
	NLIST		nl[5];
	HEADER		h;
	char		dev_nm[32], pgm_nm[64];
	char		video_mode;
	int		fd, i;

	/*
	 *	Obtém o endereço da tabela UTS do KERNEL
	 */
	nl[0].n_name = "_make_dev_nm";
	nl[1].n_name = "_make_pgm_nm";
	nl[2].n_name = "_pgversion";
	nl[3].n_name = "_video_mode";
	nl[4].n_name = NOSTR;

	if ((i = nlist (file_nm, nl)) < 0 || nl[0].n_value == 0 || nl[1].n_value == 0 || nl[2].n_value == 0)
		error ("$*Não consegui obter o endereços de \"%s\", %d", file_nm, i);

	if (nl[0].n_type != (TEXT|EXTERN) || nl[1].n_type != (TEXT|EXTERN) || nl[2].n_type != (TEXT|EXTERN))
		error ("$A área de nomes \"%s\" não está no TEXT", file_nm);

	/*
	 *	Le o HEADER do BOOT2
	 */
	if ((fd = open (file_nm, O_RDWR)) < 0)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	if (read (fd, &h, sizeof (HEADER)) != sizeof (HEADER))
		error ("$*Erro na leitura do HEADER de \"%s\"", file_nm);

	/*
	 *	Verifica se simplesmente deve imprimir os valores
	 */
	if (dev_str == NOSTR && pgm_str == NOSTR && video_str == NOSTR)
		{ pr_boot2 (file_nm, fd, &h, nl); exit (0); }

	/*
	 *	Escreve "dev_nm"
	 */
	if (dev_str == NOSTR)
		goto change_pgm;

	if (!strncmp (dev_str, "/dev/", 5))
		dev_str += 5;

	strncpy (dev_nm, dev_str, sizeof (dev_nm));
	dev_nm[sizeof (dev_nm) - 1] = '\0';

	lseek
	(	fd,
		sizeof (HEADER) + nl[0].n_value - h.h_tstart,
		SEEK_SET
	);

	if (write (fd, dev_nm, sizeof (dev_nm)) != sizeof (dev_nm))
		error ("$*Erro na escrita de \"dev_nm\" de \"%s\"", file_nm);

	/*
	 *	Escreve "pgm_nm"
	 */
    change_pgm:
	if (pgm_str == NOSTR)
		goto video_mode;

	strncpy (pgm_nm, pgm_str, sizeof (pgm_nm));
	pgm_nm[sizeof (pgm_nm) - 1] = '\0';

	lseek
	(	fd,
		sizeof (HEADER) + nl[1].n_value - h.h_tstart,
		SEEK_SET
	);

	if (write (fd, pgm_nm, sizeof (pgm_nm)) != sizeof (pgm_nm))
		error ("$*Erro na escrita de \"pgm_nm\" de \"%s\"", file_nm);

	/*
	 *	Se tem "video_mode", atribui o valor
	 */
    video_mode:
	if (video_str == NOSTR)
		goto print;

	if (nl[3].n_value != 0)
	{
		if   (streq (video_str, "113x48") || streq (video_str, "svga"))
			video_mode = 1; 
		elif (streq (video_str, "80x25")  || streq (video_str, "vga"))
			video_mode = 0; 
		else
			error ("$Modo de vídeo \"%s\" desconhecido", video_str);

		lseek
		(	fd,
			sizeof (HEADER) + nl[3].n_value - h.h_tstart,
			SEEK_SET
		);

		if (write (fd, &video_mode, sizeof (video_mode)) != sizeof (video_mode))
			error ("$*Erro na escrita do modo de vídeo de \"%s\"", file_nm);
	}

    print:
	pr_boot2 (file_nm, fd, &h, nl);

	exit (0);

}	/* end ed_boot2 */

/*
 ****************************************************************
 *	Imprime os parametros de "boot2"			*
 ****************************************************************
 */
void
pr_boot2 (const char *file_nm, int fd, HEADER *hp, NLIST *nl)
{
	char		dev_nm[32], pgm_nm[64], version_nm[64];
	char		video_mode;
	char		*cp;

	/*
	 *	Le "dev_nm"
	 */
	lseek
	(	fd,
		sizeof (HEADER) + nl[0].n_value - hp->h_tstart,
		SEEK_SET
	);

	if (read (fd, dev_nm, sizeof (dev_nm)) != sizeof (dev_nm))
	{
		error ("$*Erro na leitura de \"dev_nm\" de \"%s\"", file_nm);
	}

	/*
	 *	Le as cadeias do BOOT2
	 */
	lseek
	(	fd,
		sizeof (HEADER) + nl[1].n_value - hp->h_tstart,
		SEEK_SET
	);

	if (read (fd, pgm_nm, sizeof (pgm_nm)) != sizeof (pgm_nm))
		error ("$*Erro na leitura de \"pgm_nm\" de \"%s\"", file_nm);

	/*
	 *	Le a versão do programa
	 */
	lseek
	(	fd,
		sizeof (HEADER) + nl[2].n_value - hp->h_tstart,
		SEEK_SET
	);

	if (read (fd, version_nm, sizeof (version_nm)) != sizeof (version_nm))
		error ("$*Erro na leitura da versão de \"%s\"", file_nm);

	if ((cp = strchr (version_nm, '\n')) != NOSTR)
		cp[0] = '\0';

	if ((cp = strchr (version_nm, 'V')) == NOSTR)
		cp = version_nm;

	/*
	 *	Se tem "video_mode", obtém o valor
	 */
	if (nl[3].n_value != 0)
	{
		lseek
		(	fd,
			sizeof (HEADER) + nl[3].n_value - hp->h_tstart,
			SEEK_SET
		);

		if (read (fd, &video_mode, sizeof (video_mode)) != sizeof (video_mode))
			error ("$*Erro na leitura do modo de vídeo de \"%s\"", file_nm);
	}

	/*
	 *	Imprime os dados
	 */
	if (nl[3].n_value != 0)
	{
		printf
		(	"Arquivo \"%s\" (boot2, %s): "
			"<dev> = \"%s\", <pgm> = \"%s\", <video> = %s\n",
			file_nm, cp, dev_nm, pgm_nm, video_mode ? "113x48" : "80x25"
		);
	}
	else
	{
		printf
		(	"Arquivo \"%s\" (boot2, %s): "
			"dispositivo = \"%s\", programa = \"%s\"\n",
			file_nm, cp, dev_nm, pgm_nm 
		);
	}

}	/* end pr_boot2 */

/*
 ****************************************************************
 *	Instala o "boot0"					*
 ****************************************************************
 */
void
install_boot0 (const char *dev_nm)
{
	const char	*cp;
	int		dev_fd, boot0_fd;
	char		area[BLSZ];
	char		mbr = 1, save_mbr = 0;
	char		*boot0_ptr;
	STAT		s;
#ifdef	DEBUG
	const char	*debug_nm = "debug";
	int		debug_fd;
#endif	DEBUG

	/*
	 *	Em primeiro lugar, le o MBR e examina o que lá tem
	 */
	if ((dev_fd = open (dev_nm, O_RDWR)) < 0)
		error ("$*Não consegui abrir \"%s\"", dev_nm);

	/*
	 *	Le o bloco 0 do arquivo
	 */
	if (read (dev_fd, area, BLSZ) != BLSZ)
		error ("$*Não consegui ler de \"%s\"", dev_nm);

	/*
	 *	Procura "TROPIX"
	 */
	for (cp = area; /* vazio */; cp++)
	{
		if ((cp = memchr (cp, 'T', sizeof (area))) == NOSTR)
			break;

		if (cp > area + sizeof (area) - sizeof ("TROPIX") + 1)
			break;

		if (memeq (cp, "TROPIX", sizeof ("TROPIX") - 1))
			{ mbr = 0; break; }
	}

	/*
	 *	Pergunta se quer guardar o MBR
	 */
	if (mbr)
	{
		save_mbr++;
	}
	else
	{
		printf
		(	"O bloco 0 de \"%s\" já contém uma versão do \"boot0\".\n"
			"Guarda em \"%s\"? (n): ",
			dev_nm, mbr_nm
		);

		if (askyesno () > 0)
			save_mbr++;
	}

	/*
	 *	Salva o MBR
	 */
	if (save_mbr)
		do_save_mbr (area);

	/*
	 *	Examina o "boot0"
	 */
	if ((boot0_fd = open (boot0_nm, O_RDONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", boot0_nm);

	if (fstat (boot0_fd, &s) < 0)
		error ("$*Não consegui obter o estado de \"%s\"", boot0_nm);

	/*
	 *	Lê "boot0"
	 */
	boot0_ptr = alloca (s.st_size);

	if (read (boot0_fd, boot0_ptr, s.st_size) != s.st_size)
		error ("$*Não consegui \"%s\"", boot0_nm);

	/*
	 *	Faz o "merge" da tabela de partições
	 */
	memmove
	(	boot0_ptr + PARTITION_OFFSET,
		area + PARTITION_OFFSET,
		PARTITION_SIZE
	);

#ifdef	DEBUG
	if ((debug_fd = creat (debug_nm, 0600)) < 0)
		error ("$*Não consegui criar \"%s\"", debug_nm);

	if (write (debug_fd, boot0_ptr, s.st_size) != s.st_size)
		error ("$*Não consegui escrever o \"boot0\" em \"%s\"", debug_nm);
#else
	if (!fflag)
	{
		printf
		(	"Última chance antes de escrever o \"boot0\" em \"%s\" "
			"Continua? (n): ",
			dev_nm
		);

		if (askyesno () <= 0)
			exit (1);
	}

	if (lseek (dev_fd, 0, SEEK_SET) < 0)
		error ("$*Não consegui mover o ponteiro de leitura/escrita de \"%s\"", dev_nm);


	if (write (dev_fd, boot0_ptr, s.st_size) != s.st_size)
		error ("$*Não consegui escrever o \"boot0\" em \"%s\"", dev_nm);

	sync ();

	printf ("O programa \"boot0\" foi instalado\n");
#endif	DEBUG

} 	/* end install_boot0 */

/*
 ****************************************************************
 *	Salva o MBR						*
 ****************************************************************
 */
void
do_save_mbr (const char *area)
{
	int		mbr_fd;

	if (access (mbr_nm, F_OK) >= 0)
	{
		printf
		(	"O arquivo \"%s\" já existe. Reescreve? (n): ",
			mbr_nm
		);

		if (askyesno () <= 0)
			return;
	}

	if ((mbr_fd = creat (mbr_nm, 0600)) < 0)
		error ("$*Não consegui criar \"%s\"", mbr_nm);

	if (write (mbr_fd, area, BLSZ) != BLSZ)
		error ("$*Não consegui escrever o bloco 0 em \"%s\"", mbr_nm);

	printf ("Bloco 0 salvo em \"%s\"\n", mbr_nm);

}	/* end do_save_mbr */

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
		"%s - gerencia \"boot0\", \"boot1\" e \"boot2\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s  -0 [-wfv] [-i <índice>] [<dispositivo>]\n"
		"\t%s  -1 [-wfv] [<dispositivo>]\n"
		"\t%s [-2] [-v] [-d <dev>] [-p <pgm>] [-V <vídeo>] [<arquivo>]\n",
		pgname, pgversion, pgname, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-0: Instala/consulta/modifica o \"boot0\"\n"
		"\t-1: Instala/consulta o \"boot1\"\n"
		"\t-2: Consulta/modifica o \"boot2\"\n"
		"\t-w: Instala o \"boot0\"/\"boot1\" no <dispositivo>\n"
		"\t-f: Instala/modifica sem consultar o usuário\n"
		"\t-v: Verboso\n"
		"\t-i: A partição \"default\" do \"boot0\" será <índice>\n"
		"\t-d: O dispositivo \"default\" do \"boot2\" será <dev>\n"
		"\t-p: O programa \"default\" a executar do \"boot2\" será <pgm>\n"
		"\t-V: Usa o modo de <vídeo> dado no \"boot2\" (25x80, 113x48)\n"
		"\nObs.:\tSem opções nem argumentos, são impressos os parametros de \"/boot\"\n"
	);

	exit (2);

}	/* end help */
