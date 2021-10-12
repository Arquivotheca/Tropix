/*
 ****************************************************************
 *								*
 *			t.c					*
 *								*
 *		Testa "getmntent"				*
 *								*
 ****************************************************************
 */

#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	EVER	;;

const char		fstab_nm[] = "fstab";

entry int		vflag;

/*
 ****************************************************************
 *	Monta um sistema de arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	FILE		*fp;
	int		code;
	char		dev_nm[32], dir_nm[32], option_str[256];
	struct mntent	mntent;

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
			return (1);

		    default:			/* Erro */
			return (1);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Processa o arquivo
	 */
	if ((fp = fopen (fstab_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", fstab_nm);

	for (EVER)
	{
		if ((code = getmntent (fp, dev_nm, dir_nm, &mntent)) == EOF)
			break;

		if (code == 0)	/* Houve algum erro */
			continue;

		printf ("%-14s %-14s %s\n", dev_nm, dir_nm, editmntopt (option_str, &mntent, vflag));
	}

	fclose (fp);

	return (0);

}	/* end t */
