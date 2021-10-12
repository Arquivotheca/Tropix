/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 30.07.97				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "../h/edusr.h"

/*
 ****************************************************************
 *	Escreve uma entrada PASSWD				*
 ****************************************************************
 */
void
putpwent (const PASSWD *pw, FILE *tfp)
{
	fprintf
	(	tfp,
		"%s:%s:%s:%d:%d:%d:%d:%s:%s:%s\n",
		pw->pw_name,
		pw->pw_passwd,
		pw->pw_comment,
		pw->pw_uid,
		pw->pw_gid,
		pw->pw_quota,
		pw->pw_expir,
		pw->pw_lock,
		pw->pw_dir,
		pw->pw_shell
	);

}	/* end putpwent */

/*
 ****************************************************************
 *	Escreve uma entrada GROUP				*
 ****************************************************************
 */
void
putgrent (const GROUP *gr, FILE *tfp)
{
	fprintf
	(	tfp,
		"%s:%s:%d\n",
		gr->gr_name,
		gr->gr_passwd,
		gr->gr_gid
	);

}	/* end putgrent */

/*
 ****************************************************************
 *	Copia o arquivo						*
 ****************************************************************
 */
void
copyfile (FILE *tfp, const char *file_nm)
{
	int		fd, count;
	char		area[BLSZ];

	rewind (tfp);

	if ((fd = creat (file_nm, 0644)) < 0)
	{
		error ("*Não consegui criar o arquivo \"%s\"", file_nm);
		fclose (tfp); return;
	}

	while ((count = read (tfp->_file, area, BLSZ)) > 0)
	{
		if (write (fd, area, count) != count)
			{ count = -1; break; }
	}

	if (count < 0)
		error ("*Erro de entrada/saída durante a cópia do arquivo \"%s\"", file_nm);

	fclose (tfp); close (fd);

}	/* end copyfile */

/*
 ****************************************************************
 *	Converte uma data para o formato interno		*
 ****************************************************************
 */
time_t
time_to_internal_cv (const char *str)
{
	time_t		TEMPO;
	char		can_str[16];
	const char	*cp = str;
	char		*wp = can_str;

	/*
	 *	Analisa a data dada
	 */
	if (strlen (cp) != 8 || cp[2] != '.' || cp[5] != '.')
		return (-1);

	/*
	 *	Converte para o formato de "strtotime"
	 */
	wp[0] = cp[6]; wp[1] = cp[7];	/* Ano */
	wp[2] = cp[3]; wp[3] = cp[4];	/* Mes */
	wp[4] = cp[0]; wp[5] = cp[1];	/* Dia */
	strcpy (wp + 6, "0000");

	/*
	 *	Obtém o tempo GMT + FUSO
	 */
	time (&TEMPO);

	if ((TEMPO = strtotime (can_str, gmtime (&TEMPO))) < 0)
		return (-1);

#undef	DEBUG
#ifdef	DEBUG
	printf ("%s", brasctime (gmtime (&TEMPO)));
#endif	DEBUG

	return (TEMPO / NSPD);

}	/* end time_to_internal_cv */

/*
 ****************************************************************
 *	Converte uma data para o formato externo		*
 ****************************************************************
 */
const char *
time_to_external_cv (time_t TEMPO)
{
	const TM	*tp;
	static char	date[10];

	if (TEMPO == 0)
		return ("        ");

	TEMPO *= NSPD;

#ifdef	DEBUG
	printf ("%s", brasctime (gmtime (&TEMPO)));
#endif	DEBUG

	tp = gmtime (&TEMPO);

	sprintf (date, "%02d.%02d.%02d", tp->tm_mday, tp->tm_mon+1, tp->tm_year);

	return (date);

}	/* end time_to_internal_cv */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Fornece o "s" para o plural				*
 ****************************************************************
 */
const char *
plural (int n)
{
	if (n == 1)
		return ("");
	else
		return ("s");

}	/* end plural */
#endif	/*******************************************************/
