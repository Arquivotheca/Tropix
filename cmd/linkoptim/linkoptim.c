/*
 ****************************************************************
 *								*
 *			linkoptim.c				*
 *								*
 *	Substitui arquivos duplicados por "link"s		*
 *								*
 *	Versão	2.3.0, de 28.03.92				*
 *		3.0.1, de 19.06.97				*
 *								*
 *	Módulo: linkoptim					*
 *		Utilitários extremamente sofisticados		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ftw.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 19.06.97";

#define	elif	else if
#define	EVER	;;

#define	TMP_NM	"link_XXXXXX"

entry int	exit_code = 0;	/* Código de retorno */

entry int	fflag;		/* Force */
entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ****** Estrutura da lista de arquivos **************************
 */
typedef struct	item	ITEM;

struct	item
{
	char	*l_name;	/* Nome do arquivo */
	dev_t	l_dev;		/* Dispositivo */
	dev_t	l_ino;		/* No. do INODE */
	off_t	l_size;		/* Tamanho */
	ITEM	*l_next;
};

entry ITEM	*file_list;	/* Comeco da Lista */

/*
 ****** Protótipos de funções ***********************************
 */
int		process (const char *, STAT *, int);
void		proc_list (const char *, STAT *);
int		file_eq (const char *, const char *);
void		proc_link (const char *, const char *);
void		help (void);

/*
 ****************************************************************
 *	Substitui arquivos duplicados por "link"s		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	char		buf[512];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Force */
			fflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Prepara o terminal para consultas
	 */
	if (!fflag && (askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"/dev/tty\" (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Os argumentos supérfluos serão ignorados\n",
			pgname
		);

		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)
	{
		/*
		 *	Não foram dados argumentos: lê do "stdin"
		 */
		while (gets (buf) != NOSTR)
		{
			ftw (buf, process);
		}
	}
	else
	{
		/*
		 *	Foram dados argumentos
		 */
		for (/* vazio */; *argv; argv++)
		{
			ftw (*argv, process);
		}
	}

	return (exit_code);

}	/* end linkoptim */

/*
 ****************************************************************
 *	Processa um nome 					*
 ****************************************************************
 */
int
process (const char *file_nm, STAT *sp, int status)
{
	if   (status == FTW_NS)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		exit_code++;
		return (0);
	}
	elif (status == FTW_DNR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui percorrer \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		exit_code++;
		return (0);
	}
	elif (!S_ISREG (sp->st_mode))
	{
		return (0);
	}

	if (strncmp (file_nm, "./", 2) == 0)
		file_nm += 2;

	if (vflag)
		printf ("%s\n", file_nm);

	if (sp->st_size == 0)
		return (0);

	proc_list (file_nm, sp);

	return (0);

}	/* end process */

/*
 ****************************************************************
 *	Examina a lista de arquivos				*
 ****************************************************************
 */
void
proc_list (const char *file_nm, STAT *sp)
{
	ITEM		*lp;

	for (lp = file_list; lp != (ITEM *)NULL; lp = lp->l_next)
	{
		if (lp->l_dev == sp->st_dev && lp->l_ino == sp->st_ino)
			return;

		if (lp->l_size == sp->st_size)
		{
			if (vflag)
			{
				printf
				(	"%s :: %s (%d bytes)\n",
					file_nm, lp->l_name, lp->l_size
				);
			}

			if (file_eq (file_nm, lp->l_name))
			{
				if (sp->st_dev != lp->l_dev)
				{
					printf
					(	"Os arquivos \"%s\" e \"%s\" "
						"são iguais, mas não estão no "
						"mesmo sistema de arquivos\n",
						file_nm, lp->l_name
					);
					continue;
				}

				proc_link (lp->l_name, file_nm);

				return;
			}
		}
	}

	/*
	 *	Não encontrou nada parecido na lista; inclui
	 */
	if   ((lp = malloc (sizeof (ITEM))) == (ITEM *)NULL)
	{
		fprintf
		(	stderr,
			"%s: Não obtive memória para item de link",
			pgname
		);
	}
	elif ((lp->l_name = strdup (file_nm)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não obtive memória para item de link",
			pgname
		);
	}
	else
	{
		lp->l_dev  = sp->st_dev;
		lp->l_ino  = sp->st_ino;
		lp->l_size = sp->st_size;

		lp->l_next = file_list;
		file_list  = lp;
	}

}	/* end proc_list */

/*
 ****************************************************************
 *	Compara dois arquivos					*
 ****************************************************************
 */
int
file_eq (const char *old_nm, const char *new_nm)
{
	FILE		*old_fp, *new_fp;
	int		old_c;

	/*
	 *	Retorna 1 se os arquivos são iguais, e 0 em caso contrario
	 */
	if ((old_fp = fopen (old_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, old_nm, strerror (errno)
		);
		exit_code++;
		return (0);
	}

	if ((new_fp = fopen (new_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, new_nm, strerror (errno)
		);
		exit_code++;
		fclose (old_fp);
		return (0);
	}

	for (EVER)
	{
		if ((old_c = getc (old_fp)) != getc (new_fp))
			{ fclose (old_fp); fclose (new_fp); return (0); }

		if (old_c < 0)	/* == EOF */
			{ fclose (old_fp); fclose (new_fp); return (1); }
	}

}	/* end file_eq */

/*
 ****************************************************************
 *	Realiza o link, se desejado				*
 ****************************************************************
 */
void
proc_link (const char *old_nm, const char *new_nm)
{
	const char	*nm, *cp;
	char		*tmp_nm;
	int		len;

	if (vflag) printf
	(	"%s == %s\n",
		old_nm, new_nm
	);

	/*
	 *	Normalmente, questiona o usuário sobre a remoção
	 */
	if (!fflag)
	{
		fprintf
		(	stderr,
			"Substitui \"%s\" por um \"link\" a \"%s\"? (n): ",
			new_nm, old_nm
		);

		if (askyesno () <= 0)
			return;
	}
	/*
	 *	Compõe um nome no mesmo sistema de arquivos
	 */
	if ((cp = strrchr (new_nm, '/')) == NOSTR)
	{
		tmp_nm = TMP_NM;
	}
	else
	{
		len = cp - new_nm + 1;
		tmp_nm = alloca (len + sizeof (TMP_NM) + 1);
		strncpy (tmp_nm, new_nm, len);
		strcpy  (tmp_nm + len, TMP_NM);
	}

	/*
	 *	Dá um link de segurança
	 */
	if ((nm = mktemp (tmp_nm)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui gerar um nome temporário\n",
			pgname
		);
		exit_code++;
		return;
	}

	if (link (new_nm, nm) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar o \"link\" de segurança \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit_code++;
		return;
	}

	/*
	 *	Remove o arquivo novo
	 */
	if (unlink (new_nm) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui remover \"%s\" (%s)\n",
			pgname, new_nm, strerror (errno)
		);
		unlink (nm);
		exit_code++;
		return;
	}

	/*
	 *	Dá o link
	 */
	if (link (old_nm, new_nm) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar o novo nome \"%s\" (%s)\n",
			pgname, new_nm, strerror (errno)
		);
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" ficou com o nome \"%s\"\n",
			new_nm, nm
		);
		exit_code++;
		return;
	}

	/*
	 *	Remove o link de segurança
	 */
	if (unlink (nm) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui remover \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit_code++;
	}

}	/* end proc_link */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - substitui arquivos duplicados por \"link\"s\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-fvN] <árvore> ...\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Realiza o link (removendo o arquivo) sem consulta\n"
		"\t-v: Verboso\n"
		"\t-N: Lê os nomes das <árvores>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
	);

	exit (2);

}	/* end help */
