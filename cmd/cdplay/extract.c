/*
 ****************************************************************
 *								*
 *			extract.c				*
 *								*
 *	Extrai faixas de audio					*
 *								*
 *	Versão	3.2.3, de 27.03.00				*
 *								*
 *	Módulo: x						*
 *		Utilitários Sofisticados			*
 *		Categoria Z					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/cdio.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.2.3, de 27.03.00";

/*
 ******	Estruturas **********************************************
 */
typedef struct
{
	char	r_riff[4];	/* Nome RIFF */
	long	r_size;		/* Tamanho */

}	RIFF;

typedef struct
{
	short	f_type;		/* Tipo do som */
	short	f_channels;	/* No. de canais */
	long	f_rate;		/* Freqüência */
	long	f_buf_sz;	/* Tamanho da área recomendada */
	short	f_bytes;	/* No. de bytes por amostra */
	short	f_bits;		/* No. de bits por amostra */

}	FMT;

typedef struct
{
	int	w_offset;	/* Deslocamento */
	int	w_size;		/* Tamanho */

}	WAVE;

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry	const char	*dev_nm = "/dev/cdrom";

entry	int		start_track,
			end_track,
			vflag;

entry	int		ntracks;
entry	TOC		toc;

/*
 ****** Protótipos de funções ***********************************
 */
void		print_table (TOC *);
void		lba2msf (int, uchar *, uchar *, uchar *);
int		msf2lba (uchar, uchar, uchar);
void		print_status (TOC *, struct cd_sub_channel_info *);
void		help (int);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int				opt, fd, len, i, nframes, out_fd;
	TOC				*tp = &toc;
	TOC_ENTRY			*ep;
	struct ioc_read_audio		raudio;
	RIFF				riff;
	FMT				fmt;
	const char			*file_nm = NOSTR;
	void				*cp;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "f:s:o:vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':
			end_track = atol (optarg);
			break;

		    case 's':
			start_track = atol (optarg);
			break;

		    case 'H':			/* Help */
			help (0);

		    case 'o':
			file_nm = optarg;
			break;

		    case 'v':
			vflag++;
			break;

		    default:			/* Erro */
			putc ('\n', stderr);
			help (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (*argv != NOSTR)
		dev_nm = *argv++;

	if (*argv != NOSTR)
		error ("$Foi dado mais de um dispositivo");

	/*
	 *	Abre o dispositivo.
	 */
	if ((fd = open (dev_nm, O_RDONLY)) < 0)
		error ("$*Erro na abertura de \"%s\"", dev_nm);

	/*
	 *	Lê a TOC (Table of Contents).
	 */
	if ((len = ioctl (fd, CD_READ_TOC, &toc)) < 0)
		error ("$*Erro na leitura da TOC");

	ntracks = tp->hdr.ending_track - tp->hdr.starting_track + 1;

	/*
	 *	Faz a consistência dos números de faixas solicitados
	 */
	if (start_track == 0)
		start_track = 1;

	if (end_track == 0)
		end_track   = ntracks;

	if
	(	start_track < 1 || start_track > ntracks ||
		end_track   < 1 || end_track   > ntracks ||
		end_track   < start_track
	)
	{
		error
		(	"$Os números de faixas deveriam estar entre 1 e %d",
			ntracks
		);
	}

	/*
	 *	Verifica se todas as faixas pedidas contêm audio.
	 */
	for (i = start_track, ep = &tp->tab[i]; i <= end_track; i++, ep++)
	{
		if (ep->control & 4)
			error ("$A faixa %d não contém audio", i);
	}

	/*
	 *	Converte em blocos.
	 */
	raudio.lba = tp->tab[start_track - tp->hdr.starting_track].addr.lba;
	nframes    = tp->tab[++end_track - tp->hdr.starting_track].addr.lba - raudio.lba;

#define	NFRAMES	10

	if ((raudio.buf	= malloc (NFRAMES * CD_RAW_BLSZ)) == NULL)
		error ("$*Memória esgotada");

	/*
	 *	Cria o arquivo de saída, se o nome foi dado.
	 */
	if (file_nm == NOSTR)
		out_fd = 1;
	else if ((out_fd = creat (file_nm, 0666)) < 0)
		error ("$*Erro ao criar o arquivo \"%s\"", file_nm);

	/*
	 *	Escreve o preâmbulo do WAVE.
	 */
	strncpy (riff.r_riff, "RIFF", 4);
/***	riff.r_size = ...; */

	write (out_fd, &riff, sizeof (riff));

	write (out_fd, "WAVE", 4);

	strncpy (riff.r_riff, "fmt ", 4);
	riff.r_size = sizeof (FMT);

	write (out_fd, &riff, sizeof (riff));

	fmt.f_type	= 1;
	fmt.f_channels	= 2;
	fmt.f_rate	= 44100;
	fmt.f_buf_sz	= 1;
	fmt.f_bytes	= 4;
	fmt.f_bits	= 16;

	write (out_fd, &fmt, sizeof (fmt));

	strncpy (riff.r_riff, "data", 4);

#if (0)	/*******************************************************/
	riff.r_size = nframes * CD_RAW_BLSZ;
#endif	/*******************************************************/

	riff.r_size = nframes * 2048;

	write (out_fd, &riff, sizeof (riff));

	while (nframes > 0)
	{
		if (vflag)
			fprintf (stderr, "Restam %d blocos\n", nframes);

		raudio.nframes = (nframes > NFRAMES) ? NFRAMES: nframes;

		/*
		 *	Extrai as faixas solicitadas.
		 */
		if (ioctl (fd, CD_READ_AUDIO, &raudio) < 0)
			error ("$*Erro no ioctl (CD_READ_AUDIO)");

		/*
		 *	Despreza, em cada faixa, o prólogo e os bytes de ECC.
		 */
		for (cp = raudio.buf + 16, i = 0; i < raudio.nframes; i++, cp += CD_RAW_BLSZ)
		{
			if (write (out_fd, cp, 2048) != 2048)
				error ("$*Erro na escrita");
		}

#if (0)	/*******************************************************/
		write (out_fd, raudio.buf, raudio.nframes * CD_RAW_BLSZ);
#endif	/*******************************************************/

		raudio.lba += raudio.nframes;
		nframes    -= raudio.nframes;
	}

	close (fd);

	close (out_fd);

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - extrai faixas de CDs\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-s <faixa>] [-f <faixa>] [-o <arq>] [<dev>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: define a <faixa> inicial (default = 1)\n"
		"\t-f: define a <faixa> final   (default = última faixa)\n"
		"\t-o: gera a saída em <arq>\n"
		"\t-v: modo verboso\n"
		"\nObservações: se <dev> for omitido, o default é \"/dev/cdrom\"\n"
	);

	exit (exitval);

}	/* end help */
