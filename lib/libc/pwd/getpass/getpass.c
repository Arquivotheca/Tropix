/*
 ****************************************************************
 *								*
 *			getpass.c				*
 *								*
 *	Lê uma senha						*
 *								*
 *	Versão	1.0.0, de 28.09.86				*
 *		3.0.3, de 22.01.98				*
 *								*
 *	Módulo: getpass						*
 *		libc/pwd					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <termio.h>

/*
 ****************************************************************
 *	Variáveis e definições					*
 ****************************************************************
 */
#define	elif		else if
#define	isupper(c)	(c >= 'A' && c <='Z')

#define	CTL_Q		0x11
#define	CTL_S		0x13

#define	PWDSIZE		(80-1)

static const char	tty_nm[] = "/dev/tty";

/*
 ****************************************************************
 *	Lê uma Senha						*
 ****************************************************************
 */
char *
getpass (const char *prompt)
{
	register FILE	*fp_in, *fp_out;
	register char	*cp;
	register int 	c, upper;
	TERMIO		termio;
	static char	passwd[PWDSIZE+1];
	unsigned short	c_lflag;
	char 		t_min;

	/*
	 *	Abre o terminal
	 */
	if ((fp_in = fopen (tty_nm, "r")) == NOFILE)
		return (NOSTR);

	setbuf (fp_in, NOSTR);

	if ((fp_out = fopen (tty_nm, "w")) == NOFILE)
		{ fclose (fp_in); return (NOSTR); }

	/*
	 *	Poe o "prompt" e coloca o terminal sem ECHO
	 */
	fprintf (fp_out, "%s", prompt);	fflush (fp_out);

	ioctl (fileno (fp_in), TCLOCK);
	ioctl (fileno (fp_in), TCGETS, &termio);

	c_lflag = termio.c_lflag & (ECHO|ICANON|ISIG);
	t_min   = termio.t_min;

	termio.c_lflag &= ~(ECHO|ICANON|ISIG);
	termio.t_min    = 1;

	ioctl (fileno (fp_in), TCSETNR, &termio);
	ioctl (fileno (fp_in), TCFREE);

	if (termio.c_iflag & IUCLC)
		upper = 1;
	else
		upper = 0;

	/*
	 *	Le a senha
	 */
	for
	(	cp = passwd;
		(c = getc (fp_in)) != EOF && c != '\n' && c != '\r';
		/* vazio */
	)
	{
		if   (c == CTL_S || c == CTL_Q)
		{
			continue;
		}
		elif (c == termio.t_kill)
		{
			cp = passwd;
		}
		elif (cp < &passwd[PWDSIZE])
		{
			if (upper && isupper (c))
				c += 'a' - 'A';

			*cp++ = c;
		}
	}

	*cp = 0;

	putc ('\n', fp_out);

	/*
	 *	Restaura o ECHO (isto é, o estado original)
	 */
	ioctl (fileno (fp_in), TCLOCK);
	ioctl (fileno (fp_in), TCGETS, &termio);

	termio.c_lflag |= c_lflag;
	termio.t_min    = t_min;

	ioctl (fileno (fp_in), TCSETNR, &termio);
	ioctl (fileno (fp_in), TCFREE);

	fclose (fp_in);
	fclose (fp_out);

	return (passwd);

}	/* end getpass */
