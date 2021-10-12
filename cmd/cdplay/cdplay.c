/*
 ****************************************************************
 *								*
 *			cdplay.c				*
 *								*
 *	Controla o CDROM					*
 *								*
 *	Versão	3.2.3, de 17.03.00				*
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
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.2.3, de 17.03.00";

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry	const char	*dev_nm = "/dev/cdrom";

entry	int		start_track,
			end_track,
			eject,
			pause_resume_stop,
			table,
			advance;

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
	int				opt, fd, i, save_argc;
	TOC				*tp = &toc;
	TOC_ENTRY			*ep;
	struct cd_sub_channel_info	s;
	struct ioc_play_blocks		play;

	if ((save_argc = argc) == 1)
	{
		if (argv[0][6] == '+')
			advance = 1;
		elif (argv[0][6] == '-')
			advance = -1;
	}

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ef:prs:tNH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':
			eject++;
			break;

		    case 'f':
			end_track = atol (optarg);
			break;

		    case 'p':
			if (pause_resume_stop == 1)
				pause_resume_stop = 3;
			else
				pause_resume_stop = 1;
			break;

		    case 'r':
			pause_resume_stop = 2;
			break;

		    case 's':
			start_track = atol (optarg);
			break;

		    case 't':
			table++;
			break;

		    case 'H':			/* Help */
			help (0);

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
	 *	Abre o dispositivo e tenta ler a TOC (Table of Contents).
	 */
	if ((fd = open (dev_nm, O_RDONLY)) < 0)
		error ("$*Erro na abertura de \"%s\"", dev_nm);

	if (eject)
	{
		if
		(	ioctl (fd, CD_LOCK_UNLOCK, 0) < 0 ||
			ioctl (fd, CD_START_STOP, 1) < 0 ||
			ioctl (fd, CD_START_STOP, 2) < 0
		)
			error ("$*Erro no \"ioctl (CD_EJECT)\"");

		close (fd);
		return (0);
	}

	if (pause_resume_stop)
	{
		if (pause_resume_stop == 3)
		{
			if (ioctl (fd, CD_START_STOP, 0) < 0)
				error ("$*Erro no \"ioctl\"");
		}
		else
		{
			if (ioctl (fd, CD_PAUSE_RESUME, pause_resume_stop - 1) < 0)
				error ("$*Erro no \"ioctl\"");
		}

		close (fd);
		return (0);
	}

	/*
	 *	Lê a TOC (Table of Contents).
	 */
	if ((ntracks = ioctl (fd, CD_READ_TOC, &toc)) < 0)
		error ("$*Erro na leitura da TOC");

/***	ntracks = tp->hdr.ending_track - tp->hdr.starting_track + 1; ***/

	if (table)
	{
		print_table (tp);
		close (fd);
		return (0);
	}

	if (save_argc == 1)
	{
		if (ioctl (fd, CD_READ_SUBCHANNEL, &s) < 0)
			return (-1);

		if
		(	s.header.audio_status == CD_AS_PLAY_IN_PROGRESS ||
			s.header.audio_status == CD_AS_PLAY_PAUSED
		)
		{
			if (advance != 0)
			{
				start_track = s.what.position.track_number + advance;
			}
			else
			{
				print_status (tp, &s);
				close (fd);
				return (0);
			}
		}
	}

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
	play.blk = tp->tab[start_track - tp->hdr.starting_track].addr.lba;
	play.len = tp->tab[++end_track - tp->hdr.starting_track].addr.lba - play.blk;

	/*
	 *	Toca as faixas solicitadas.
	 */
	if (ioctl (fd, CD_PLAY_BLOCKS, &play) < 0)
		error ("$*Erro no ioctl (CD_PLAY_BLOCKS)");

	close (fd);

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Imprime o conteúdo do CD				*
 ****************************************************************
 */
void
print_table (TOC *tp)
{
	int		first, block, prev, len, data = 0;
	TOC_ENTRY	*ep;
	uchar		m, s, f;

	printf ("FAIXA  D/A    INÍCIO     TAM     DURAÇÃO\n");

	ep = &tp->tab[0];

	if (ep->addr_type == CD_LBA_FORMAT)
	{
		first =	ep->addr.lba;
	}
	else
	{
		first =	msf2lba
			(	ep->addr.msf.minute,
				ep->addr.msf.second,
				ep->addr.msf.frame
			);
	}

	for (prev = first, ++ep; ep <= &tp->tab[ntracks]; ep++)
	{
		printf
		(	"%4d    %c   ",
			(ep - 1)->track,
			(ep - 1)->control & 4 ? 'D' : 'A'
		);

		if (ep->addr_type == CD_LBA_FORMAT)
		{
			block =	ep->addr.lba;
		}
		else
		{
			block =	msf2lba
				(	ep->addr.msf.minute,
					ep->addr.msf.second,
					ep->addr.msf.frame
				);
		}

		len = block - prev;

		printf ("%8d %8d ", prev, len);

		if (((ep - 1)->control & 4) == 0)
		{
			lba2msf (len, &m, &s, &f);
			printf ("%6d'%02d\"", m, s);
		}
		else
		{
			data++;
		}

		putchar ('\n');

		prev = block;
	}

	len = tp->tab[ntracks].addr.lba - first;

	printf ("TOTAL       %8d %8d ", first, len);

	if (!data)
	{
		lba2msf (len, &m, &s, &f);
		printf ("%6d'%02d\"", m, s);
	}

	putchar ('\n');

}	/* end print_table */

/*
 ****************************************************************
 *	Converte LBA em MSF					*
 ****************************************************************
 */
void
lba2msf (int lba, uchar *m, uchar *s, uchar *f)
{
	lba  = (lba + 150) & 0xFFFFFF;
	*m   = lba / (60 * 75);
	lba %= (60 * 75);
	*s   = lba / 75;
	*f   = lba % 75;

}	/* end lba2msf */

/*
 ****************************************************************
 *	Converte MSF em LBA					*
 ****************************************************************
 */
int
msf2lba (uchar m, uchar s, uchar f)
{
	return (m * 60 + s) * 75 + f - 150;

}	/* end msf2lba */

/*
 ****************************************************************
 *	Obtém e imprime o "status"				*
 ****************************************************************
 */
void
print_status (TOC *tp, struct cd_sub_channel_info *sp)
{
	struct cd_sub_channel_position_data	*dp;
	uchar		min, sec, frame, tmin, tsec, tfra;
	TOC_ENTRY	*ep;

	dp = &sp->what.position;

	switch (sp->header.audio_status)
	{
	    case CD_AS_AUDIO_INVALID:
		printf ("Estado inválido\n");
		break;

	    case CD_AS_PLAY_IN_PROGRESS:
		lba2msf (long_endian_cv (dp->reladdr.lba), &min, &sec, &frame);

		ep = &tp->tab[dp->track_number - 1];

		if (ep->addr_type == CD_LBA_FORMAT)
		{
			lba2msf
			(	(ep + 1)->addr.lba - ep->addr.lba,
				&tmin, &tsec, &tfra
			);
		}
		else
		{
			tmin = ep->addr.msf.minute;
			tsec = ep->addr.msf.second;
			tfra = ep->addr.msf.frame;
		}

		printf
		(	"Reproduzindo a faixa %d (%d'%02d\" de %d'%02d\")\n",
			dp->track_number,
			min, sec, tmin, tsec
		);

		break;

	    case CD_AS_PLAY_PAUSED:
		lba2msf (long_endian_cv (dp->reladdr.lba), &min, &sec, &frame);

		ep = &tp->tab[dp->track_number - 1];

		if (ep->addr_type == CD_LBA_FORMAT)
		{
			lba2msf
			(	(ep + 1)->addr.lba - ep->addr.lba,
				&tmin, &tsec, &tfra
			);
		}
		else
		{
			tmin = ep->addr.msf.minute;
			tsec = ep->addr.msf.second;
			tfra = ep->addr.msf.frame;
		}

		printf
		(	"A reprodução da faixa %d foi suspensa (%d'%02d\" de %d'%02d\")\n",
			dp->track_number,
			min, sec, tmin, tsec
		);

		break;

	    case CD_AS_PLAY_COMPLETED:
		printf ("Reprodução completada com sucesso\n");
		break;

	    case CD_AS_PLAY_ERROR:
		printf ("Reprodução completada com erro\n");
		break;

	    case CD_AS_NO_STATUS:
		printf ("Não há reprodução em andamento\n");
		break;
	}

}	/* end print_status */

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
		"%s - reproduz faixas de CDs\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-epr] [-s <faixa>] [-f <faixa>] [<dev>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-e: ejeta o CD\n"
		"\t-p: interrompe a reprodução  (pause)\n"
		"\t-r: recomeça   a reprodução  (resume)\n"
		"\t-s: define a <faixa> inicial (default = 1)\n"
		"\t-f: define a <faixa> final   (default = última faixa)\n"
		"\t-t: imprime o conteúdo do CD\n"
		"\nObservações: se <dev> for omitido, o default é \"/dev/cdrom\"\n"
	);

	exit (exitval);

}	/* end help */
