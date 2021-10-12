/*
 ****************************************************************
 *								*
 *			nm_expand.c				*
 *								*
 *	Expande os nomes em argumentos				*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 27.02.96				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
#define	MAX_NM	100

#define	ARG_INC	10		/* Incremento da lista de argumentos */

#define	NO_ITEM (ITEM *)NULL

/*
 ******	Variáveis globais da expansão DOS ***********************
 */
entry int		expand_arg_sz;		/* Número de entradas */
entry const char	**expand_argv;		/* Começo */
entry const char	**expand_argp;		/* Próximo */
entry const char	**expand_end_argv;	/* Final */

/*
 ******	Protótipos de funções ***********************************
 */
int		dos_recursive_expansion (const char *, const char *, int);
int		sofix_recursive_expansion (const char *, const char *, int, int);
int		expand_arg_deliver (const char *);

/*
 ****************************************************************
 *	Expande os nomes no DOS					*
 ****************************************************************
 */
void		
dos_nm_expand (void (*ftn) (int, const char *[]), const char *argv[])
{
	const char	*arg;
	char		*cp;
	int		not_found = 0;
	const char	**old_expand_argp;
	char		c;

	/*
	 *	Pequena consistência
	 */
	if (expand_arg_sz > 0)
		error ("dos_nm_expand: expand_arg_sz = %d", expand_arg_sz);

	/*
	 *	O primeiro argumento é o nome do comando
	 */
	if (expand_arg_deliver (*argv++) < 0)
		return;

	/*
	 *	Examina os demais argumentos
	 */
	for (/* vazio */; arg = *argv; argv++)
	{
		old_expand_argp = expand_argp;

		if   ((c = arg[0]) == '\'' || c == '"')
		{
			cp = strend (arg) - 1;

			if (cp[0] != c)
			{
				printf
				(	"%s: Cadeia (%s) não terminada\n",
					cmd_nm, arg
				);
				return;
			}

			cp[0] = '\0';

			if (expand_arg_deliver (arg + 1) < 0)
				return;
		}
		elif (strpbrk (arg, "?*[]!") == NOSTR)
		{
			if (expand_arg_deliver (arg) < 0)
				return;
		}
		elif (arg[0] == '/')
		{
			if (dos_recursive_expansion (arg, arg + 1, 0) < 0)
				return;
		}
		else
		{
			if (dos_recursive_expansion (arg, arg, cwd_cluster) < 0)
				return;
		}

		if (old_expand_argp == expand_argp)
		{
			printf
			(	"%s: Padrão (\"%s\") não encontrado\n",
				cmd_nm, arg
			);
			not_found++;
		}
	}

	/*
	 *	Verifica se teve algum erro
	 */
	if (not_found)
		return;

	/*
	 *	Coloca o ponteiro NULO no final
	 */
	if (expand_arg_deliver (NOSTR) < 0)
		return;

	/*
	 *	Finalmente, chama a função desejada
	 */
	(*ftn) (expand_argp - expand_argv - 1, expand_argv);

	/*
	 *	Libera os argumentos
	 */
   /***	expand_arg_free (); ***/

}	/* end dos_nm_expand */

/*
 ****************************************************************
 *	Função de expansão recursiva DOS			*
 ****************************************************************
 */
int
dos_recursive_expansion (const char *path, const char *next, int clusno)
{
	typedef struct	item	ITEM;
	struct	item 	{ char	*i_nm; int i_clusno; ITEM *i_next; };
	ITEM		*ip, *lp;
	ITEM		file_list;
	char		*next_next;
	char		*new_path, *new_next_next;
	int		len;
	DOSFILE		f;
	DOSSTAT		z;
	char		pattern[LFN_SZ];

#undef	DEBUG
#ifdef	DEBUG
	printf ("expansion (1): \"%s\", \"%s\", %d\n", path, next, clusno);
#endif	DEBUG

	/*
	 *	Primeira parte: obtém o padrão
	 */
	if ((next_next = strchr (next, '/')) != NOSTR)
		*next_next++ = '\0';

	strcpy (pattern, next);

	if (next_next != NOSTR)
		next_next[-1] = '/';

	len = next - path;

	/*
	 *	Segunda parte: lê e ordena os nomes escolhidos do diretório
	 */
	file_list.i_next = NO_ITEM;

	if (dos_open_by_clus (&f, clusno, BIG_SZ) < 0)
		return (-1);

	dos_lfn_init (&z);

	while (dos_read (&f, &z.z_d, sizeof (DOSDIR)) > 0)
	{
		if (z.z_name[0] == '\0')
			break;

		if (z.z_name[0] == Z_EMPTY)
			{ dos_lfn_reset (&z); continue; }

		if (Z_ISLFN (z.z_mode))
			{ dos_lfn_add (&z); continue; }

		if (z.z_mode & Z_HIDDEN)
			{ dos_lfn_reset (&z); continue; }

		dos_lfn_get_nm (&z);

		if (z.z_lfn_nm[0] == '.')
			{ dos_lfn_reset (&z); continue; }

		if (!patmatch (z.z_lfn_nm, pattern))
			{ dos_lfn_reset (&z); continue; }

		/*
		 *	Insere na lista - repare que NÃO DIR tem clusno < 0
		 */
		GET_CLUSTER (&z);

		ip = alloca (sizeof (ITEM));

		ip->i_nm = alloca (strlen (z.z_lfn_nm) + 1);
		strcpy (ip->i_nm, z.z_lfn_nm);

	   	if (Z_ISDIR (z.z_mode))
		   	ip->i_clusno = z.z_cluster;
		else
		   	ip->i_clusno = -1;

		for (lp = &file_list; lp->i_next != NO_ITEM; lp = lp->i_next)
		{
			if (strcmp (z.z_lfn_nm, lp->i_next->i_nm) <= 0)
				break;
		}

		ip->i_next = lp->i_next;
		lp->i_next = ip;

		dos_lfn_reset (&z);

	}	/* end lendo o diretório */

	/*
	 *	Terceira parte: percorre a lista, expandindo
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
	   /***	len = next - path; ***/
		new_path = alloca (strlen (path) + LFN_SZ);

		if (len > 0)
			strncpy (new_path, path, len);

		strcpy (new_path + len, ip->i_nm);

		if   (next_next == NOSTR)
		{
			if (expand_arg_deliver (new_path) < 0)
				return (-1);
		}
		elif (ip->i_clusno >= 0)	/* == Z_ISDIR (d.d_mode)) */
		{
			new_next_next = strend (new_path) + 1;
			strcat (new_path, next_next - 1);

			if (dos_recursive_expansion (new_path, new_next_next, ip->i_clusno) < 0)
				return (-1);
		}

	}	/* end percorrendo o diretório */

	return (0);

}	/* end dos_recursive_expansion */

/*
 ****************************************************************
 *	Expande os nomes no TROPIX				*
 ****************************************************************
 */
void		
sofix_nm_expand (void (*ftn) (int, const char *[]), const char *argv[])
{
	const char	*arg;
	char		*cp;
	int		not_found = 0, have_cwd = 0;
	const char	**old_expand_argp;
	char		c;
	STAT		s;

	/*
	 *	Pequena consistência
	 */
	if (expand_arg_sz > 0)
		error ("sofix_nm_expand: expand_arg_sz = %d", expand_arg_sz);

	/*
	 *	O primeiro argumento é o nome do comando
	 */
	if (expand_arg_deliver (*argv++) < 0)
		return;

	/*
	 *	Examina os demais argumentos
	 */
	for (/* vazio */; arg = *argv; argv++)
	{
		old_expand_argp = expand_argp;

		/*
		 *	Verifica se é literal
		 */
		if   ((c = arg[0]) == '\'' || c == '"')
		{
			cp = strend (arg) - 1;

			if (cp[0] != c)
			{
				printf
				(	"%s: Cadeia (%s) não terminada\n",
					cmd_nm, arg
				);
				return;
			}

			cp[0] = '\0';

			if (expand_arg_deliver (arg + 1) < 0)
				return;

			continue;
		}

		/*
		 *	Verifica se tem "$"
		 */
		if (strchr (arg, '$'))
		{
			cp = alloca (strlen (arg) + 256);
			arg = env_var_scan (cp, (char *)arg);
		}

		/*
		 *	Agora verifica se tem de expandir
		 */
		if   (strpbrk (arg, "?*[]!") == NOSTR)
		{
			if (expand_arg_deliver (arg) < 0)
				return;
		}
		elif (arg[0] == '/')
		{
			if (sofix_recursive_expansion (arg, arg + 1, root_dev, root_ino) < 0)
				return;
		}
		else
		{
			if (have_cwd++ == 0 && stat (".", &s) < 0)
			{
				error ("sofix_nm_expand: Não consegui obter o estado de \".\"");
					return;
			}

			if (sofix_recursive_expansion (arg, arg, s.st_dev, s.st_ino) < 0)
				return;
		}

		if (old_expand_argp == expand_argp)
		{
			printf
			(	"%s: Padrão (\"%s\") não encontrado\n",
				cmd_nm, arg
			);
			not_found++;
		}
	}

	/*
	 *	Verifica se teve algum erro
	 */
	if (not_found)
		return;

	/*
	 *	Coloca o ponteiro NULO no final
	 */
	if (expand_arg_deliver (NOSTR) < 0)
		return;

	/*
	 *	Finalmente, chama a função desejada
	 */
	(*ftn) (expand_argp - expand_argv - 1, expand_argv);

	/*
	 *	Libera os argumentos
	 */
   /***	expand_arg_free (); ***/

}	/* end sofix_nm_expand */

/*
 ****************************************************************
 *	Função de expansão recursiva TROPIX			*
 ****************************************************************
 */
int
sofix_recursive_expansion (const char *path, const char *next, int dev, int ino)
{
	typedef struct	item	ITEM;
	struct	item 	{ char	i_nm[IDSZ+2]; ITEM *i_next; };
	ITEM		*ip, *lp;
	ITEM		file_list;
	char		*next_next, nm[IDSZ+2];
	char		*new_path, *new_next_next;
	int		len;
	char		pattern[32];
	DIR		*dir_fp;
	const DIRENT	*dp;
	STAT		s;

	/*
	 *	Primeira parte: obtém o padrão
	 */
	nm[IDSZ] = '\0';

	if ((next_next = strchr (next, '/')) != NOSTR)
		*next_next++ = '\0';

	strcpy (pattern, next);

	if (next_next != NOSTR)
		next_next[-1] = '/';

	len = next - path;

	/*
	 *	Segunda parte: lê e ordena os nomes escolhidos do diretório
	 */
	file_list.i_next = NO_ITEM;

	if ((dir_fp = inopendir (dev, ino)) == NODIR)
		return (0);

	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
			continue;

		memmove (nm, dp->d_name, IDSZ);

		if (!patmatch (nm, pattern))
			continue;

		/*
		 *	Insere na lista
		 */
		ip = alloca (sizeof (ITEM));

		strcpy (ip->i_nm, nm);

		for (lp = &file_list; lp->i_next != NO_ITEM; lp = lp->i_next)
		{
			if (strcmp (nm, lp->i_next->i_nm) <= 0)
				break;
		}

		ip->i_next = lp->i_next;
		lp->i_next = ip;

	}	/* end lendo o diretório */

	closedir (dir_fp);

	/*
	 *	Terceira parte: percorre a lista, expandindo
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
	   /***	len = next - path; ***/
		new_path = alloca (strlen (path) + IDSZ + 2);

		if (len > 0)
			strncpy (new_path, path, len);

		strcpy (new_path + len, ip->i_nm);

		if   (next_next == NOSTR)
		{
			if (expand_arg_deliver (new_path) < 0)
				return (-1);
		}
		elif (stat (new_path, &s) >= 0 && S_ISDIR (s.st_mode))
		{
			new_next_next = strend (new_path) + 1;
			strcat (new_path, next_next - 1);

			if (sofix_recursive_expansion (new_path, new_next_next, s.st_dev, s.st_ino) < 0)
				return (-1);
		}

	}	/* end percorrendo o diretório */

	return (0);

}	/* end sofix_recursive_expansion */

/*
 ****************************************************************
 *	Adiciona um argumento na lista de argumentos		*
 ****************************************************************
 */
int
expand_arg_deliver (const char *arg)
{
	const char	**old_expand_argv;

#ifdef	DEBUG
	printf ("expansion (3): \"%s\"\n", arg);
#endif	DEBUG

	/*
	 *	Obtém a memória necessária
	 */
	if   (expand_arg_sz == 0)
	{
		if ((expand_argv = malloc (ARG_INC * sizeof (char *))) == NOVOID)
			goto bad;

		expand_end_argv = expand_argv + ARG_INC;
		expand_argp = expand_argv;
		expand_arg_sz = ARG_INC;
	}
	elif (expand_argp >= expand_end_argv)
	{
		old_expand_argv = expand_argv;
		expand_arg_sz += ARG_INC;

		if ((expand_argv = realloc (expand_argv, expand_arg_sz * sizeof (char *))) == NOVOID)
			goto bad;

		expand_end_argv = expand_argv + expand_arg_sz;
		expand_argp += expand_argv - old_expand_argv;
	   /***	expand_arg_sz = ...; ***/
	}

	/*
	 *	Insere
	 */
	if (arg != NOSTR && (arg = strdup (arg)) == NOSTR)
		goto bad;

	*expand_argp++ = arg;

	return (0);

	/*
	 *	Acabou a memória
	 */
    bad:
	error ("nm_expand: Memória esgotada");

   /***	expand_arg_free (); ***/

	return (-1);

}	/* end expand_arg_deliver */

/*
 ****************************************************************
 *	Libera a área ocupada pelos argumentos expandidos	*
 ****************************************************************
 */
void
expand_arg_free (void)
{
	if (expand_arg_sz > 0)
	{
		while (expand_argp > expand_argv)
		{
			const char	*cp;

			if ((cp = *--expand_argp) != NOSTR)
				free ((char *)cp);
		}

		free (expand_argv);
	}

	expand_arg_sz = 0;

}	/* end expand_arg_free */

/*
 ****************************************************************
 *	Imprime os nomes expandidos				*
 ****************************************************************
 */
void
do_echo (int argc, const char *argv[])
{
	int		i;

	if   (streq (argv[1], "-H"))
	{
		fprintf
		(	stderr,
			"%s - ecoa os argumentos (expandidos no %s)\n"
			"\nSintaxe:\n"
			"\t%s [-d] <arg> ...\n",
			cmd_nm, cmd_nm[4] == 'p' ? sys_nm : "DOS", cmd_nm
		);

		fprintf
		(	stderr,
			"\nOpções:"
			"\t-d: Formato vetorial (ao invés de linear)\n"
		);
	}
	elif (streq (argv[1], "-d"))
	{
		printf ("argc      = %d\n", argc);

		for (i = 0; i <= argc; i++)
			printf ("argv[%2d] = \"%s\"\n", i, argv[i]);
	}
	else
	{
		for (i = 1; i < argc; i++)
			printf ("%s ", argv[i]);
		printf ("\n");
	}

}	/* end do_echo */
