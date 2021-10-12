/*
 ****************************************************************
 *								*
 *			grent.c					*
 *								*
 *	Leitura do arquivo "/etc/group"				*
 *								*
 *	Versão	1.0.0, de 28.10.86				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: grent						*
 *		libc/pwd					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>

/*
 ****************************************************************
 *	Variaveis Globais					*
 ****************************************************************
 */
static FILE	*_grfp;		/* Fp do Arquivo */

/*
 ****************************************************************
 *	Prepara para a leitura de "/etc/group"			*
 ****************************************************************
 */
void
setgrent (void)
{
	if (_grfp == NOFILE)
		_grfp = fopen ("/etc/group", "r");

	if (_grfp == NOFILE)
		return;

	rewind (_grfp);

}	/* end setgrent */

/*
 ****************************************************************
 *	Le uma entrada e converte 				*
 ****************************************************************
 */
GROUP *
getgrent (void)
{
	char		c;
	char		*cp;
	static GROUP	*grp;
	static char	*grbuf;

	/*
	 *	Prólogo
	 */
	if (grbuf == NOSTR && (grbuf = malloc (120)) == NOSTR)
		return (NOGROUP);

	if (grp == NOGROUP && (grp = malloc (sizeof (GROUP))) == NOGROUP)
		return (NOGROUP);

	if (_grfp == NOFILE)
		_grfp = fopen ("/etc/group", "r");

	if (_grfp == NOFILE)
		return (NOGROUP);

	do
	{
		if (fngets (grbuf, 120, _grfp) == NOSTR)
			return (NOGROUP);
	}
	while ((c = grbuf[0]) == '\0' || c == '#');

	/*
	 *	Inicialmente, zera os campos
	 */
	grp->gr_name   = "";
	grp->gr_passwd = "";
	grp->gr_gid    = 0;
	grp->gr_mem    = (char **)NULL;		/* NÃO implementado */

	/*
	 *	Em seguida, prepara os campos
	 */
	if ((cp = strfield (grbuf, ':')) != NOSTR)
		grp->gr_name = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		grp->gr_passwd = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		grp->gr_gid = atoi (cp);

   /***	if ((cp = strfield (NOSTR, ':')) != NOSTR) ***/
	   /***	grp->gr_mem = ...; ***/

	return (grp);

}	/* end getgrent */

/*
 ****************************************************************
 *	Encerra a Leitura do Arquivo "/etc/group"		*
 ****************************************************************
 */
void
endgrent (void)
{
	fclose (_grfp);

	_grfp = NOFILE;

}	/* end endgrent */

/*
 ****************************************************************
 *	Obtém a entrada group dado o no. do groupo		*
 ****************************************************************
 */
GROUP *
getgrgid (int gid)
{
	GROUP		*grp;

	setgrent ();

	while ((grp = getgrent ()) != NOGROUP)
	{
		if (grp->gr_gid == gid)
			return (grp);
	}

	return (NOGROUP);

}	/* end getgrgid */

/*
 ****************************************************************
 *	Obtem a entrada "group" dado o nome do grupo		*
 ****************************************************************
 */
GROUP *
getgrnam (const char *name)
{
	GROUP		*grp;

	setgrent ();

	while ((grp = getgrent ()) != NOGROUP)
	{
		if (streq (grp->gr_name, name))
			return (grp);
	}

	return (NOGROUP);

}	/* end gegrnam */
