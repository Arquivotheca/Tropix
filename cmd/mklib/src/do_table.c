/*
 ****************************************************************
 *								*
 *			do_table.c				*
 *								*
 *	Imprime uma tabela sobre a biblioteca			*
 *								*
 *	Versão	1.0.0, de 23.10.86				*
 *		4.0.0, de 20.08.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <time.h>
#include <pwd.h>
#include <a.out.h>

#include "../h/mklib.h"

/*
 ****************************************************************
 *	Imprime uma tabela sobre a biblioteca			*
 ****************************************************************
 */
int
do_table (const char *argv[])
{
	const MOD	*mp;
	const char	*mod_nm;

	/*
	 *	Lê o arquivo de sinopse
	 */
	read_sinop_file (0);

	/*
	 *	Verifica se foram dados nomes de módulos
	 */
	if (*argv == NOSTR)
	{
		for (mp = mod_first; mp != NOMOD; mp = mp->m_next)
			list_mod (mp);
	}
	else	 	/* Lista apenas os modulos dados */
	{
		while ((mod_nm = *argv++) != NOSTR)
		{
			if ((mp = mod_search (mod_nm)) == NOMOD)
				error ("Não encontrei o módulo \"%s\"", mod_nm);
			else
				list_mod (mp);
		}
	}

	vflag = 0;	/* Não escreve a mensagem sobre a sinopse */

	return (0);	/* Não escreve a sinopse */

}	/* end do_table */

/*
 ****************************************************************
 *	Imprime os símbolos de um módulo			*
 ****************************************************************
 */
void
list_mod (const MOD *mp)
{
	const SYMTB	*zp;
	const char	*cp, *tp;
	STAT		s;
	char		first = 1;

	printf ("\n");

	if (vflag)
	{
		if (instat (lib_dev, mp->m_ino, &s) >= 0)
		{
			cp = modetostr (s.st_mode);
			tp = btime (&s.st_mtime);

			printf
			(	"%s %-14.14s %6d  %-5.5s %-15.15s  ", 
				cp + 14, pwcache (s.st_uid),
				s.st_size, tp, tp + 9
			);
		}
	}

	printf (mp->m_mod_nm);

	/*
	 *	Imprime os símbolos
	 */
	if (vflag)
		printf ("\n\nSímbolos: ");
	else
		printf (": ");

	for (zp = mp->m_sym; zp != NOSYMTB; zp = zp->z_sym_next)
	{
		if (first)
			first = 0;
		else
			printf (", ");

		printf ("%s", zp->z_sym_nm);
	}

	printf ("\n");

}	/* end list_mod */
