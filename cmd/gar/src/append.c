/*
 ****************************************************************
 *								*
 *			append.c				*
 *								*
 *	General Archiver -- Utilitario para Fitas/Disquettes	*
 *								*
 *	Versão	2.3.0, de 19.08.89				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#include "../h/common.h"
#include "../h/gar.h"
#include "../h/reduce.h"

/*
 ****************************************************************
 *	Acréscimo						*
 ****************************************************************
 */
void
append (const char *argv[])
{
	KHEAD		*kh = &khead;

	/*
	 *	Analisa o tipo da coleção
	 */
	prein ();

	/*
	 *	"Append" pode ser feito apenas com formato GAR
	 */
	if (type != GAR || Tflag)
		error ("$A operação de acréscimo só é possível com o formato GAR");

	/*
	 *	Pula até o final da coleção
	 */
	while (getheader () >=  0)
	{
		if (vflag || nflag)
		{
			fprintf (stderr, "%c ", (modetostr (kh->k_mode))[0]);

			if (S_ISBLK (kh->k_mode) || S_ISCHR (kh->k_mode))
				fprintf (stderr, "  %2d/%-2d * ", MAJOR (kh->k_rdev), MINOR (kh->k_rdev)); 
			else
				fprintf (stderr, "%7d * ", kh->k_origsz ? kh->k_origsz : kh->k_size);

			print_file_nm (kh->k_name, &kh->k_stat, NOSTR, stderr);

			if (kh->k_origsz)
			{
				put_tabs (kh->k_name, stderr);

				fprintf
				(	stderr,
					"(%s %%, %s%d)",
					editpercent (getpercent (kh->k_origsz - kh->k_size, kh->k_origsz)),
					kh->k_method == 'D' ? "D " : "", kh->k_bits
				);
			}

		}

		inskip (0);

		if (vflag || nflag)
			fprintf (stderr, "\n");
	}

	/*
	 *	Volta o "trailer"
	 */
	if (bback (sizeof (GHEAD), 1 /* read/write */) < 0)
	{
		fprintf (stderr, "Não foi possível adicionar arquivos no final da coleção\n");
		quit (1);
	}

	out (argv);

	quit (0);

}	/* end append */
