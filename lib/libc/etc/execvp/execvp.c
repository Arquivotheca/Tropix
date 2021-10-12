/*
 ****************************************************************
 *								*
 *			execvp.c				*
 *								*
 *	Interpreta as regras de busca do "exec"			*
 *								*
 *	Versão	2.1.0, de 23.07.88				*
 *		3.2.3, de 07.01.00				*
 *								*
 *	Módulo: execv						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	elif	else if

/*
 ****************************************************************
 *	Executa, dado a lista de argumento			*
 ****************************************************************
 */
int
execlp (const char *file, const char *argv, ...)
{
	execvp (file, &argv);
	return (-1);

}	/* end execlp */

/*
 ****************************************************************
 *	Executa, dado um ponteiro para a lista de argumentos	*
 ****************************************************************
 */
int
execvp (const char *file, const char *argp[])
{
	const char	*pp, *shell, *sp;
	char		*cp;
	const char	**ap, **new_argp;
	char		eaccess = 0;
	int		n;
	char		path[512];

	/*
	 *	Obtem os prefixos de caminhos
	 */
	if   (strchr (file, '/'))
		pp = ".";
	elif ((pp = getenv ("PATH")) == NOSTR)
		pp = ".:/bin:/usr/bin";

	/*
	 *	Tenta os vários prefixos
	 */
	while (*pp)
	{
		/*
		 *	Prepara um prefixo
		 */
		for (cp = path; *pp && (*cp = *pp++) != ':'; cp++)
			/* vazio */;

		if (cp != path)
			*cp++ = '/';

		/*
		 *	Adiciona o nome do arquivo dado
		 */
		strcpy (cp, file);

		/*
		 *	Tira o "./" do início
		 */
		if (!strncmp (cp = path, "./", 2))
			cp += 2;
#undef	DEBUG
#ifdef	DEBUG
		/*
		 *	Apenas para depuração
		 */
		printf ("	\"%s\"\n", cp); continue;
#endif	DEBUG
		/*
		 *	Tenta executar
		 */
		execv (cp, argp);

		/*
		 *	Não conseguiu; analisa porque
		 */
		switch (errno)
		{
			/*
			 *	Tenta através do "sh"
			 */
		    case ENOEXEC:
			for (n = sizeof (char *), ap = argp; *ap != NOSTR; ap++)
				n += sizeof (char *);

			new_argp = alloca (n + sizeof (char *));

			memmove (new_argp + 1, argp, n);

			if ((shell = getenv ("SHELL")) == NOSTR)
				shell = "/bin/sh";

			if ((sp = strrchr (shell, '/')) == NOSTR)
				sp = shell;
			else
				sp++;

			new_argp[0] = sp;
			new_argp[1] = cp;
#ifdef	DEBUG
			error ("%s, %s, %s", shell, new_argp[0], new_argp[1]);
#endif	DEBUG
			execv (shell, new_argp);
			errno = ENOEXEC;
		    case ENOMEM:
		    case E2BIG:
		    case EOLDVER:
		    case ENOSHLIB:
		    case ETXTBSY:
			return (-1);

		    case EACCES:
			eaccess++;
			break;

		}	/* end switch */

	}	/* end while */	

	/*
	 *	Se houve algum erro de caminho anterior 
	 */
	if (eaccess)
		errno = EACCES;

	return (-1);

}	/* end execvp */
