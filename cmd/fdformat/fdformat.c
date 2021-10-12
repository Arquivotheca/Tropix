/*
 ****************************************************************
 *								*
 *			fdformat.c				*
 *								*
 *	Formata disquetes					*
 *								*
 *	Versão	3.0.0, de 05.02.96				*
 *		4.2.0, de 13.05.02				*
 *								*
 *	Módulo: fdformat					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kcntl.h>
#include <sys/disktb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 13.05.02";

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	fflag;		/* Força */
entry int	tflag;		/* Tipo atual */

/*
 ******	Tabela de tipos de disquetes ****************************
 */
typedef struct
{
	char	*t_nm;		/* Nome da entrada */
	int	t_sect;		/* Setores por trilha */
	int	t_gap;		/* Espaço entre setores */
	int	t_cyl;		/* No. total de cilindros */
	int	t_size;		/* No. total de setores */
	int	t_stepcyl;	/* Log (2) da rel. cilindro geométrico/lógico */
	int	t_trans;	/* Código da velocidade de transferencia */
	int	t_spec;		/* Velocidade de "seek" e cabeças */
	int	t_format_gap;	/* Espaço entre setores usado na formatação */
	int	t_interleave;	/* No. de setores a pular na formatação */

}	FD_TYPE;

#define	NO_FD_TYPE	(FD_TYPE *)NULL

#define	NHEAD	2	/* Todos tipos têm 2 cabeças */
#define	SECSIZE	2	/* Todos tipos têm setores de 512 bytes */
#define	DATALEN	0xFF	/* Todos tipos têm o mesmo tamanho de dados */

/*
 ******	Tabela de nomes de tamanhos conhecidos ******************
 */
typedef struct
{
	char	*s_nm;		/* Nome do tamanho */
	int	s_size;		/* Tamanho */

}	SIZE_NM;

const SIZE_NM	size_nm[] =
{
	"1722",		3444,	/* Na realidade 1.68 MB */
	"1.72",		3444,

	"1440",		2880,	/* Na realidade 1.40 MB */
	"1.44",		2880,

	"1200",		2400,
	"1.2",		2400,

	"720",		1440,
	"0.72",		1440,
	".72",		1440,

	"360",		720,
	"0.36",		720,
	".36",		720,

	"",		0
};

/*
 ******	Tabela do "interleave" **********************************
 */
#define	MAX_SECT 21	/* Número máximo de setores */

entry char	interleave_tb[MAX_SECT];

/*
 ******	Imagem da informação para a operação "format track" *****
 */
typedef struct
{
	char	i_cyl_no;	/* No. do cilindro */
	char	i_head_no;	/* No. da cabeça */
	char	i_sec_no;	/* No. do setor (começando com 1) */
	char	i_sec_size;	/* Tamanho do setor (512 => 2) */

}	IMAGE;

/* Cuidado para não atravessar fronteira de 64 KB */

entry IMAGE	*image;

/*
 ****** Protótipos de funções ***********************************
 */
void		print_present_format (const char *);
void		help (void);

/*
 ****************************************************************
 *	Formata disquetes					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd, n;
	FD_TYPE		*fp;
	const SIZE_NM	*sp;
	const char	*dev_str = NOSTR, *size_str = NOSTR;
	const char	*inter_str = NOSTR;
	FD_TYPE		*fd_types;
	int		pos, sect, track;
	const char	*str;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "01s:e:ftvH")) != EOF)
	{
		switch (opt)
		{
		    case '0':			/* fd0 */
			if (dev_str != NOSTR)
				help ();

			dev_str = "/dev/rfd0";
			break;

		    case '1':			/* fd1 */
			if (dev_str != NOSTR)
				help ();

			dev_str = "/dev/rfd1";
			break;

		    case 's':			/* Tamanho */
			if (size_str != NOSTR)
				help ();

			size_str = optarg;
			break;

		    case 'e':			/* Entrelaçamento */
			if (inter_str != NOSTR)
				help ();

			inter_str = optarg;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'f':			/* Força */
			fflag++;
			break;

		    case 't':			/* Tipo atual */
			tflag++;
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

	/*
	 *	Obtém o nome do dispositivo
	 */
	if (dev_str == NOSTR)
		{ dev_str = argv[0]; argc--; }

	if (argc != 0)
		help ();

	if (size_str == NOSTR)
		size_str = "1440";

	/*
	 *	Verifica se é só para imprimir o formato atual
	 */
	if (tflag)
		{ print_present_format (dev_str); exit (exit_code); }

	/*
	 *	Abre o dispositivo
	 */
	if ((fd = open (dev_str, O_WRONLY|O_FORMAT|O_LOCK)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, dev_str, strerror (errno)
		);
		exit (1);
	}

	if (fstat (fd, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, dev_str, strerror (errno)
		);
		exit (1);
	}

	if (!S_ISCHR (s.st_mode))
	{
		fprintf
		(	stderr,
			"%s: O dispositivo \"%s\" NÃO é de caracteres\n",
			pgname, dev_str
		);
		exit (1);
	}

	/*
	 *	Obtém a tabela de tipos de disquetes
	 */
	if ((n = ioctl (fd, FD_GET_TYPE_TB, NOVOID)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o tamanho da tabela de tipos (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	fd_types = alloca (n * sizeof (FD_TYPE));

	if (ioctl (fd, FD_GET_TYPE_TB, fd_types) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter a tabela de tipos (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

#undef	DEBUG
#ifdef	DEBUG
	/*
	 *	Imprime alguns dados da tabela
	 */
	printf ("n = %d\n", n);

	for (fp = fd_types; fp->t_size != 0; fp++)
		printf ("size = %d\n", fp->t_size);
#endif	DEBUG

	/*
	 *	Procura o nome do tamanho na tabela de tamanhos
	 */
	for (sp = size_nm; /* abaixo */; sp++)
	{
		if (sp->s_size == 0)
		{
			fprintf
			(	stderr,
				"%s: Tamanho inválido: \"%s\"\n",
				pgname, size_str
			);
			exit (1);
		}

		if (streq (sp->s_nm, size_str))
			break;
	}

#ifdef	DEBUG
	printf ("size = %d\n", sp->s_size);
#endif	DEBUG

	/*
	 *	Agora, procura o tamanho na tabela de discos
	 */
	for (fp = fd_types; /* abaixo */; fp++)
	{
		if (fp->t_size == 0)
		{
			fprintf
			(	stderr,
				"%s: Tamanho inválido: \"%s\"\n",
				pgname, size_str
			);
			exit (1);
		}

		if (fp->t_size == sp->s_size)
			break;
	}

#ifdef	DEBUG
	printf ("size = %d\n", fp->t_size);
#endif	DEBUG

	/*
	 *	Processa o entrelaçamento dado
	 */
	if (inter_str != NOSTR)
	{
		if
		(	(n = strtol (inter_str, &str, 0)) <= 0 ||
			n >= fp->t_sect || *str != '\0'
		)
		{
			fprintf
			(	stderr,
				"%s: Fator de entrelaçamento \"%s\" inválido\n",
				pgname, inter_str
			);
			exit (1);
		}

		fp->t_interleave = n;
	}

	/*
	 *	Aloca memória para IMAGE
	 *
	 *	O tamanho tem de ser bem maior para satisfazer "physio"
	 *	Além disto, os (4 * MAX_SECT) bytes iniciais não devem
	 *	cruzar a fronteira dos 64 KB
	 */
	if ((image = malloc (MAX_SECT * BLSZ)) == NOVOID)
	{
		fprintf
		(	stderr,
			"%s: Não consegui aloca memórica\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Fornece ao "driver" o tipo desejado do disquete
	 */
	if (ioctl (fd, FD_PUT_FORMAT_TYPE, fp) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui atribuir o tipo do disquete (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Prepara o "interleave"
	 */
	for (pos = 0, sect = 1; sect <= fp->t_sect; sect++)
	{
		while (interleave_tb[pos] != 0)
		{
			if (++pos >= fp->t_sect)
				pos = 0;
		}

		interleave_tb[pos] = sect;

		if ((pos += fp->t_interleave) >= fp->t_sect)
			pos -= fp->t_sect;
	}

#ifdef	DEBUG
	printf ("interleave = %d (", fp->t_interleave);

	for (pos = 0; pos < fp->t_sect; pos++)
		printf ("%d ", interleave_tb[pos]);

	printf (")\n");
#endif	DEBUG

	/*
	 *	Completa os campos constantes da imagem
	 */
	for (pos = 0; pos < fp->t_sect; pos++)
	{
	   /***	image[pos].i_cyl_no   = ...; /* Abaixo ***/
	   /***	image[pos].i_head_no  = ...; /* Abaixo ***/
		image[pos].i_sec_no   = interleave_tb[pos];
		image[pos].i_sec_size = SECSIZE;
	}

	/*
	 *	Detalhes da formatação
	 */
	if (vflag)
	{
		printf
		(	"Setores por trilha   = %d\n"
			"Cilindros            = %d\n"
			"No. total de setores = %d\n"
			"Entrelaçamento       = %d\n",
			fp->t_sect, fp->t_cyl, fp->t_size, fp->t_interleave
		);

		if (!fflag)
			printf ("\n");
	}

	/*
	 *	Última chance ...
	 */
	if (!fflag)
	{
		fprintf
		(	stderr,
			"Última chance antes de escrever em \"%s\". "
			"Continua? (n): ",
			dev_str
		);

		if (askyesno () <= 0)
			exit (0);
	}

	/*
	 *	Agora formata trilha por trilha
	 */
	for (track = 0; track < NHEAD * fp->t_cyl; track++)
	{
		/* Campos restantes */

		for (pos = 0; pos < fp->t_sect; pos++)
		{
		   	image[pos].i_cyl_no   = track / NHEAD;
		   	image[pos].i_head_no  = track % NHEAD;
		   /***	image[pos].i_sec_no   = ...; /* Acima ***/
		   /***	image[pos].i_sec_size = ...; /* Acima ***/
		}

		if (write (fd, image, fp->t_sect * BLSZ) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de formatação de uma trilha (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}

		printf
		(	"\rFormatando a trilha %d (%d %%) ",
			track,
			100 * (track + 1) / (NHEAD * fp->t_cyl)
		);
		fflush (stdout);
	}

	printf ("\n");

	/*
	 *	Epílogo: Volta as cabeças para o início
	 */
	close (fd); fd = open (dev_str, O_RDONLY); read (fd, image, BLSZ);

	return (exit_code);

}	/* end fdformat */

/*
 ****************************************************************
 *	Imprime o tipo de formatação atual			*
 ****************************************************************
 */
void
print_present_format (const char *dev_str)
{
	int			fd;
	const SIZE_NM		*sp;
	const char		*size_str = NOSTR;
	STAT			s;
	DISKTB			ud;

	/*
	 *	Abre o dispositivo
	 */
	if ((fd = open (dev_str, O_RDONLY)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, dev_str, strerror (errno)
		);
		exit (1);
	}

	if (fstat (fd, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, dev_str, strerror (errno)
		);
		exit (1);
	}

	if (!S_ISCHR (s.st_mode) && !S_ISBLK (s.st_mode))
	{
		fprintf
		(	stderr,
			"%s: O dispositivo \"%s\" NÃO é de caracteres nem de blocos\n",
			pgname, dev_str
		);
		exit (1);
	}

	/*
	 *	Obtém alguns dados
	 */
	if (kcntl (K_GET_DEV_TB, s.st_rdev, &ud) < 0)
	{
		printf
		(	"%s: Não consegui obter o tamanho do dispositivo \"%s\"\n",
			pgname, dev_str
		);
		return;
	}

	/*
	 *	Procura o nome do tamanho na tabela de tamanhos
	 */
	for (sp = size_nm; sp->s_size != 0; sp++)
	{
		if (sp->s_size == ud.p_size)
			{ size_str = sp->s_nm; break; }
	}

	/*
	 *	Imprime os dados
	 */
	printf
	(	"%d setores por trilha, "
		"%d cilindros, "
		"%d setores",
		ud.p_sect, ud.p_cyl, ud.p_size
	);

	if (size_str != NOSTR)
		printf (" (%s KB)\n", size_str);
	else
		printf ("\n");

}	/* end print_present_format */

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
		"%s - formata disquetes\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-fvt] [-s <tamanho>] [-e <entre>] [-01] [<dispositivo>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Não consulta o usuário antes de iniciar a formatação\n"
		"\t-v: Dá alguns parametros de formatação\n"
		"\t-t: Não formata - apenas identifica a formatação atual\n"
		"\t-s: Tamanho do disquete: 1720, 1440, 1200, 720, 360 KB\n"
		"\t    (se esta opção não for dada, será assumido 1440 KB)\n"
		"\t-e: Usa o fator de entrelaçamento <entre> ao invés do padrão\n"
		"\t-0: O <dispositivo> usado é \"/dev/rfd0\"\n"
		"\t-1: O <dispositivo> usado é \"/dev/rfd1\"\n"
	);

	exit (2);

}	/* end help */
