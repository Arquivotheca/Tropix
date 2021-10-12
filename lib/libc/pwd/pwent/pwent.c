/*
 ****************************************************************
 *								*
 *			pwent.c					*
 *								*
 *	Leitura do arquivo de contas/senhas			*
 *								*
 *	Versão	1.0.0, de 18.09.86				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: pwent						*
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
#include <pwd.h>

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
#define	 PWD_FILE	"/etc/passwd"

static FILE	*_pwfp;		/* Fp do Arquivo */
static int	_pw_slot = -1;	/* O No. da entrada no arquivo */

/*
 ****************************************************************
 *	Prepara para a Leitura de "/etc/passwd"			*
 ****************************************************************
 */
void
setpwent (void)
{
	if (_pwfp == NOFILE)
		_pwfp = fopen (PWD_FILE, "r");

	if (_pwfp == NOFILE)
		return;

	_pw_slot = -1;

	rewind (_pwfp);

}	/* end setpwent */

/*
 ****************************************************************
 *	Le uma entrada e converte 				*
 ****************************************************************
 */
PASSWD *
getpwent (void)
{
	char		c;
	char		*cp;
	static PASSWD	*pwp;
	static char	*_pwbuf;

	/*
	 *	Prólogo
	 */
	if (_pwbuf == NOSTR && (_pwbuf = malloc (120)) == NOSTR)
		return (NOPASSWD);

	if (pwp == NOPASSWD && (pwp = malloc (sizeof (PASSWD))) == NOPASSWD)
		return (NOPASSWD);

	if (_pwfp == NOFILE)
		setpwent ();

	if (_pwfp == NOFILE)
		return (NOPASSWD);

	do
	{
		if (fngets (_pwbuf, 120, _pwfp) == NOSTR)
			return (NOPASSWD);
	}
	while ((c = _pwbuf[0]) == '\0' || c == '#');

	_pw_slot++;

	/*
	 *	Inicialmente, zera os campos
	 */
	pwp->pw_name	= "";
	pwp->pw_passwd	= "";
	pwp->pw_comment	= "";
	pwp->pw_uid	= 0;
	pwp->pw_gid	= 0;
	pwp->pw_quota	= 0;
	pwp->pw_expir	= 0;
	pwp->pw_lock	= "";
	pwp->pw_dir	= "";
	pwp->pw_shell	= "";

	/*
	 *	Em seguida, prepara os campos
	 */
	if ((cp = strfield (_pwbuf, ':')) != NOSTR)
		pwp->pw_name = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_passwd = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_comment = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_uid = atoi (cp);

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_gid = atoi (cp);

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_quota = atoi (cp);

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_expir = atoi (cp);

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_lock = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_dir = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		pwp->pw_shell = cp;

	return (pwp);

}	/* end getpwent */

/*
 ****************************************************************
 *	Encerra a leitura do arquivo "/etc/passwd"		*
 ****************************************************************
 */
void
endpwent (void)
{
	fclose (_pwfp);

	_pw_slot = -1;

	_pwfp = NOFILE;

}	/* end endpwent */

/*
 ****************************************************************
 *	Obtem a entrada passwd dado o no. do usuário		*
 ****************************************************************
 */
PASSWD	*
getpwuid (int uid)
{
	PASSWD		*pwp;

	setpwent ();

	while ((pwp = getpwent()) != NOPASSWD)
	{
		if (pwp->pw_uid == uid)
			return (pwp);
	}

	return (NOPASSWD);

}	/* end getpwuid */

/*
 ****************************************************************
 *	Obtem a entrada passwd dado o nome do usuário		*
 ****************************************************************
 */
PASSWD	*
getpwnam (const char *name)
{
	PASSWD		*pwp;

	setpwent ();

	while ((pwp = getpwent()) != NOPASSWD)
	{
		if (streq (pwp->pw_name, name))
			return (pwp);
	}

	return (NOPASSWD);

}	/* end gepwnam */

/*
 ****************************************************************
 *	Obtem a posição do arquivo				*
 ****************************************************************
 */
off_t
getpwpos (void)
{
	return (ftell (_pwfp));

}	/* end getpwpos */

/*
 ****************************************************************
 *	Obtem a posição da entrada no arquivo			*
 ****************************************************************
 */
int
getpwslot (void)
{
	return (_pw_slot);

}	/* end getpwpos */
