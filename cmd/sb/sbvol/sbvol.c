/*
 ****************************************************************
 *								*
 *			sbvol.c					*
 *								*
 *	Controla o volume da placa de som SB-16			*
 *								*
 *	Versão	3.2.3, de 04.03.00				*
 *		3.2.3, de 04.03.00				*
 *								*
 *	Módulo: sbvol						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/ioctl.h>

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
const char	pgversion[] =  "Versão: 3.2.3, de 04.03.00";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Lista de registros do "mixer" ***************************
 */
#define	SB_MASTER_VOL_L_INDEX	0x30
#define	SB_MASTER_VOL_R_INDEX	0x31
#define	SB_VOICE_VOL_L_INDEX	0x32
#define	SB_VOICE_VOL_R_INDEX	0x33
#define	SB_CD_VOL_L_INDEX	0x36
#define	SB_CD_VOL_R_INDEX	0x37
#define	SB_OUTPUT_GAIN_L_INDEX	0x41
#define	SB_OUTPUT_GAIN_R_INDEX	0x42

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Controla o volume da placa de som SB-16			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd;
	int		vol, vol_left, vol_right;
	const char	*str, *vol_str = NOSTR;
	int		data[2];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "s:vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 's':			/* Atribui o volume */
			vol_str = optarg;
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
	 *	Abre a placa de som
	 */
	if ((fd = open ("/dev/sb", O_WRONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", "/dev/sb");

	/*
	 *	Le o volume atual
	 */
	if ((vol_left = ioctl (fd, SB_READ_MIXER, SB_MASTER_VOL_L_INDEX)) < 0)
		error ("$*Não consegui obter o volume do canal esquerdo");

	if ((vol_right = ioctl (fd, SB_READ_MIXER, SB_MASTER_VOL_R_INDEX)) < 0)
		error ("$*Não consegui obter o volume do canal direito");

	vol = (vol_left + vol_right) >> 1;

	/*
	 *	Verifica se é "sbvol+" ou "sbvol-"
	 */
	str = strend (pgname) - 1;

	if   (str[0] == '+')
	{
		vol += 8;
	}
	elif (str[0] == '-')
	{
		vol -= 8;
	}
	elif (vol_str != NOSTR)
	{
		if ((vol = strtol (vol_str, &str, 0)) < 0 || vol > 100)
			error ("$Volume \"%s\" inválido", vol_str); 

		vol = vol * 255 / 100;
	}
	else
	{
		printf ("Volumes = (%d %%, %d %%)\n", vol_left * 100 / 248, vol_right * 100 / 248);
		return (exit_code);
	}

	/*
	 *	Atribui o volume
	 */
	if   (vol > 255)
		data[0] = 255;
	elif (vol < 0)
		data[0] = 0;
	else
		data[0] = vol;

	data[1] = SB_MASTER_VOL_L_INDEX;

	if (ioctl (fd, SB_WRITE_MIXER, data) < 0)
		error ("$*Não consegui atribui o volume do canal esquerdo");

	data[1] = SB_MASTER_VOL_R_INDEX;

	if (ioctl (fd, SB_WRITE_MIXER, data) < 0)
		error ("$*Não consegui atribui o volume do canal direito");

	/*
	 *	Le novamente o volume atual
	 */
	if ((vol_left = ioctl (fd, SB_READ_MIXER, SB_MASTER_VOL_L_INDEX)) < 0)
		error ("$*Não consegui obter o volume do canal esquerdo");

	if ((vol_right = ioctl (fd, SB_READ_MIXER, SB_MASTER_VOL_R_INDEX)) < 0)
		error ("$*Não consegui obter o volume do canal direito");

	printf ("Volumes = (%d %%, %d %%)\n", vol_left * 100 / 248, vol_right * 100 / 248);

	return (exit_code);

}	/* end sbvol */

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
		"%s - controla o volume da placa de som SB-16\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-s <vol>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: Atribui o volume <vol>\n"
		"\t-v: Verboso\n"
		"\nObs.:\t\"%s+\" aumenta o volume de aproximadamente 5 %%\n"
		"\t\"%s-\" diminui o volume de aproximadamente 5 %%\n",
		pgname, pgname
	);

	exit (2);

}	/* end help */
