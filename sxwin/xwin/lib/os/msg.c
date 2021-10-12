/*
 ****************************************************************
 *								*
 *			msg.c					*
 *								*
 *	Imprime mensagens de erro				*
 *								*
 *	Versão	1.0.0, de 18.03.97				*
 *		1.0.1, de 27.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

extern const char	*pgname;

/*
 ****************************************************************
 *	Emite uma mensagem de erro				*
 ****************************************************************
 */
entry void
msg (char *format, ...)
{
	va_list		ap;
	int		doexit = 0, doperror = 0;
	static int	pid = 0, is_a_tty = -1;

	/*
	 *	Obtém (só da primeira vez) algumas informações.
	 */
	if (pid == 0)
		pid = getpid ();

	if (is_a_tty < 0)
		is_a_tty = isatty (fileno (stderr));

	/*
	 *	Processa os caracteres especiais no início do formato.
	 */
	if (*format == '$')
		format++, doexit++;

	if (*format == '*')
		format++, doperror++;

	/*
	 *	Imprime a mensagem adequadamente.
	 */
	if (is_a_tty)
		fprintf (stderr, "%s: ", pgname);
	else
		fprintf (stderr, "[%s] %s(%d): ", fulltime (), pgname, pid);

	va_start (ap, format);
	vfprintf (stderr, format, ap);
	va_end (ap);

	if (doperror && errno != NOERR)
		fprintf (stderr, " (%s)", strerror (errno));

	putc ('\n', stderr);

	fflush (stderr);

	if (doexit)
		exit (1);

}	/* end msg */
