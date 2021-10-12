/*
 ****************************************************************
 *								*
 *			getlogin.c				*
 *								*
 *	Obtem o nome do usuário, dado no "login"		*
 *								*
 *	Versão	1.0.0, de 27.10.86				*
 *		4.6.0, de 12.10.04				*
 *								*
 *	Modulo: getlogin					*
 *		libc/pwd					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <utmp.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
static const char	utmp_nm[] = "/var/adm/utmp"; /* Nome do Arquivo */

/*
 ****************************************************************
 *	Obtem o nome de LOGIN					*
 ****************************************************************
 */
char *
getlogin (void)
{
	const char		*tty_nm;
	FILE			*fp;
	UTMP			utmp;
	static char		login_nm[16];

	/*
	 *	Obtem o nome do terminal associado
	 */
	if ((tty_nm = ttyname (2)) == NOSTR)
		return (NOSTR);

	tty_nm += 5;	/* Pula "/dev/" */

	/*
	 *	Procura no arquivo UTMP
	 */
	if ((fp = fopen (utmp_nm, "r")) == NOFILE)
		return (NOSTR);

	while (fread (&utmp, sizeof (UTMP), 1, fp) == 1)
	{
		if (streq (tty_nm, utmp.ut_line))
			{ fclose (fp); strcpy (login_nm, utmp.ut_name); return (login_nm); }
	}

	fclose (fp); return (NOSTR);

}	/* end getlogin */
