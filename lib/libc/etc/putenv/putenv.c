/*
 ****************************************************************
 *								*
 *			putenv.c				*
 *								*
 *	Modifica ou acresenta um valor ao ambiente		*
 *								*
 *	Versão	1.0.0, de 28.10.86				*
 *		3.0.0, de 30.05.95				*
 *								*
 *	Módulo: putenv						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#undef	DEBUG

#ifdef	DEBUG
#define	ENV_SZ_INC	3	/* Incremento do tamanho do ambiente */
#else
#define	ENV_SZ_INC	50	/* Incremento do tamanho do ambiente */
#endif	DEBUG

#define	elif	else if

static int	environ_sz;	/* Tamanho atual do ambiente criado */

static const char **environ_copy; /* Cópia do "environ" para cheque */

/*
 ****************************************************************
 *	Modifica ou acrescenta uma variável ao ambiente		*
 ****************************************************************
 */
int
putenv (register const char *str)
{
	register int		sz;
	register const char	**ep, *cp;

	/*
	 *	Verifica se cadeia é da forma var=valor
	 */
	if ((cp = strchr (str, '=')) == NOSTR)
		return (-1);

	sz = cp - str + 1;	/* Incorpora o "=" */

	/*
	 *	Verifica se já existe um ambiente
	 */
	if (environ == (const char **)NULL)
	{
#ifdef	DEBUG
		printf ("Criando o primeiro ambiente\n");
#endif	DEBUG
		if ((ep = malloc ((ENV_SZ_INC + 1) * sizeof (char *))) == NOVOID)
			return (-1);

		environ_sz   = ENV_SZ_INC;
		environ_copy = environ = ep;

		*ep++ = str; *ep = NOSTR;

		return (0);
	}

	/*
	 *	Verifica se a variável já se encontra no ambiente (NOSSO ou não)
	 */
	for (ep = environ; *ep != NOSTR; ep++)
	{
		if (strncmp (*ep, str, sz) == 0)
			{ *ep = str; return (0); }
	}

	/*
	 *	NÃO está no ambiente - verifica se o ambiente já é o NOSSO
	 */
	if (environ != environ_copy || environ_sz == 0)
	{
#ifdef	DEBUG
		printf ("Copiando o ambiente original\n");
#endif	DEBUG
		sz = ep - environ;	/* Espaço original */

		if ((ep = malloc ((sz + ENV_SZ_INC + 1) * sizeof (char *))) == NOVOID)
			return (-1);

		memmove (ep, environ, sz * sizeof (char *));

		environ_sz   = sz + ENV_SZ_INC;
		environ_copy = environ = ep;

		ep += sz; *ep++ = str; *ep = NOSTR;

		return (0);
	}

	/*
	 *	NÃO está no NOSSO ambiente - verifica se tem espaço para colocar
	 */
	if (ep >= environ + environ_sz)
	{
#ifdef	DEBUG
		printf ("Aumentando o ambiente próprio\n");
#endif	DEBUG
		sz = ep - environ;	/* Espaço original */

		if ((ep = realloc (environ, (environ_sz + ENV_SZ_INC + 1) * sizeof (char *))) == NOVOID)
			return (-1);

		environ_sz  += ENV_SZ_INC;
		environ_copy = environ = ep;

		ep += sz;
	}
#ifdef	DEBUG
	else
	{
		printf ("Coube no ambiente próprio\n");
	}
#endif	DEBUG

	*ep++ = str; *ep = NOSTR;

	return (0);

}	/* end putenv */
