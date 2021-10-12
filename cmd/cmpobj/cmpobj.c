/*
 ****************************************************************
 *								*
 *			cmpobj.c				*
 *								*
 *	Compara pares de módulos objeto				*
 *								*
 *	Versão	1.0.0, de 13.03.87				*
 *		4.6.0, de 26.08.04				*
 *								*
 *	Módulo: cmpobj						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <a.out.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "Versão:	4.6.0, de 26.08.04";

#define	elif	else if

entry char	*dirend;	/* Ponteiro para o final do nome do dir. */

entry int	qflag;		/* Quick */
entry int	dflag;		/* Debug */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

const char	**color_vector;	/* Para as cores dos modos */

const char	*error_color_str;
const char	*warn_color_str;
const char	*def_color_str;

/*
 ****** Tabela de comparação ************************************
 */
typedef struct
{
	int		c_offset;
	int		c_size;
	const char	*c_text;

}	COMPAR;

#define	offset_and_size(type, member)	offsetof (type, member), sizeof (((type *)0)->member)

const COMPAR	compar_vec[] =
{
	offset_and_size (HEADER, h_machine),	"\t%sOs computadores diferem%s: (%04X :: %04X)\n",
	offset_and_size (HEADER, h_magic),	"\t%sOs números mágicos diferem%s: (%04X :: %04X)\n",
	offset_and_size (HEADER, h_version),	"\t%sAs versões diferem%s: (%d :: %d)\n",
	offset_and_size (HEADER, h_flags),	"\t%sOs indicadores diferem%s: (%04X :: %04X)\n",
   /***	offset_and_size (HEADER, h_time),	"\t%sOs tempos diferem%s: (%08X :: %08X)\n", ***/
	offset_and_size (HEADER, h_serial),	"\t%sOs números seriais diferem%s: (%d :: %d)\n",
	offset_and_size (HEADER, h_tstart),	"\t%sOs inícios dos TEXTs diferem%s: (%P :: %P)\n",
	offset_and_size (HEADER, h_dstart),	"\t%sOs inícios dos DATAs diferem%s: (%P :: %P)\n",
	offset_and_size (HEADER, h_entry),	"\t%sOs pontos de entrada diferem%s: (%P :: %P)\n",
	0, 0
};

/*
 ******	Protótipos de funções ***********************************
 */
int		praeludium (int, const char **, STAT *, FILE **, HEADER *);
void		data_cmp (const char *, const char *, HEADER *, HEADER *);
int		compare (const char *, const char *);
int		section_cmp (const char *, off_t, FILE *, FILE *, off_t, off_t);
void		header_endian_conversion (HEADER *);
void		help (void);

/*
 ****************************************************************
 *	Compara pares de módulos objeto				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ret = 0;
	const char	*cp;
	char		*pathname = NOSTR;
	const char	*dir_nm;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "qdNH")) != EOF)
	{
		switch (opt)
		{
		    case 'q':			/* Quick */
			qflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
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

	if (argc < 1)
		help ();

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stderr)), (void *)NULL, &color_vector);

	error_color_str = color_vector[MC_ERR];
	warn_color_str  = color_vector[MC_WARN];
	def_color_str   = color_vector[MC_DEF];

	/*
	 *	Examina se o último argumento é um diretório
	 */
	if (stat ((dir_nm = argv[argc-1]), &s) >= 0 && S_ISDIR (s.st_mode))
	{
		pathname = alloca (1024);

		strcpy (pathname, dir_nm); strcat (pathname, "/");
		dirend = strrchr (pathname, '/') + 1;

		if (dirend [-2] == '/')
			dirend--;
	}

	/*
	 *	Analisa o Nflag
	 */
	if   (argc == 1)
		Nflag++;
	elif (Nflag)
		error ("Os argumentos supérfluos serão ignorados");

	if ((Nflag || argc > 2)  &&  dirend == NOSTR)
		error ("$O arquivo \"%s\" não é um diretório", dir_nm);

	/*
	 *	Processa os argumentos
	 */
	if   (dirend == NOSTR) 		/* Primeiro caso: "cmpobj a b" */
	{
		ret += compare (argv[0], argv[1]);
	}
	elif (!Nflag) 			/* Segundo caso: "cmpobj a b c ... dir" */
	{
		argv[argc-1] = NOSTR;

		for (/* vazio */; *argv; argv++)
		{
			if ((cp = strrchr (*argv, '/')) == NOSTR)
				cp = *argv;
			else
				cp++;

			strcpy (dirend, cp);

			ret += compare (*argv, pathname);
		}
	}
	else				/* Terceiro caso: "cmpobj -N dir" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			if ((cp = strrchr (area, '/')) == NOSTR)
				cp = area;
			else
				cp++;

			strcpy (dirend, cp);

			ret += compare (area, pathname);
		}
	}

	return (ret);

}	/* end main */

/*
 ****************************************************************
 *	Compara um par de arquivos				*
 ****************************************************************
 */
int
compare (const char *old_nm, const char *new_nm)
{
	int		ret;
	STAT		old_s,   new_s;
	HEADER		old_h,   new_h;
	FILE		*old_fp, *new_fp;
	off_t		old_off, new_off;
	const COMPAR	*cp;

	if (dflag)
		fprintf (stderr, "=> compare (%s, %s)\n", old_nm, new_nm);

	/*
	 *	Analisa os dois arquivos
	 */
	if (ret = praeludium (0, &old_nm, &old_s, &old_fp, &old_h))
		return (ret);

	if (ret = praeludium (1, &new_nm, &new_s, &new_fp, &new_h))
		{ fclose (old_fp); return (ret); }

	/*
	 *	Verifica se se trata na realidade, do mesmo arquivo
	 */
	if (old_s.st_dev == new_s.st_dev && old_s.st_ino == new_s.st_ino)
	{
		fprintf
		(	stderr,
			"\t\"%s\" e \"%s\" são o mesmo arquivo\n",
			old_nm, new_nm
		);

		fclose (old_fp); fclose (new_fp); return (0);
	}

	/*
	 *	Comparação rápida
	 */
	if (qflag && old_h.h_time == new_h.h_time && old_s.st_size == new_s.st_size)
		{ fclose (old_fp); fclose (new_fp); return (0); }

	/*
	 *	Compara diversos campos
	 */
	for (cp = compar_vec; cp->c_size != 0; cp++)
	{
		ulong		old, new;

		if (cp->c_size == sizeof (ushort))
		{
			old = *(ushort *)((char *)&old_h + cp->c_offset);
			new = *(ushort *)((char *)&new_h + cp->c_offset);
		}
		else
		{
			old = *(ulong *)((char *)&old_h + cp->c_offset);
			new = *(ulong *)((char *)&new_h + cp->c_offset);
		}

		if (old != new)
			{ fprintf (stderr, cp->c_text, error_color_str, def_color_str, old, new); ret++; }
	}

#undef	OTHER_FIELDs
#ifdef	OTHER_FIELDs	/* As comparações abaixo já estão na TABELA, acima */
	/*
	 *	Compara os computadores
	 */
	if (old_h.h_machine != new_h.h_machine)
	{
		fprintf
		(	stderr,
			"\t%sOs computadores diferem%s: (%04X :: %04X)\n",
			error_color_str, def_color_str, old_h.h_machine, new_h.h_machine
		);
		ret++;
	}

	/*
	 *	Compara as versões
	 */
	if (old_h.h_version != new_h.h_version)
	{
		fprintf
		(	stderr,
			"\t%sAs versões diferem%s: (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_version, new_h.h_version
		);
		ret++;
	}
#endif	OTHER_FIELDs

	/*
	 *	Compara os TEXTs
	 */
	old_off = sizeof (HEADER);
	new_off = sizeof (HEADER);

	if (old_h.h_tsize != new_h.h_tsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos dos TEXTs diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_tsize, new_h.h_tsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"TEXT", old_h.h_tsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_tsize;
	new_off += new_h.h_tsize;

	/*
	 *	Compara os DATAs
	 */
	if (old_h.h_dsize != new_h.h_dsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos dos DATAs diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_dsize, new_h.h_dsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"DATA", old_h.h_dsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_dsize;
	new_off += new_h.h_dsize;

	/*
	 *	Compara os BSSs
	 */
	if (old_h.h_bsize != new_h.h_bsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos dos BSSs  diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_bsize, new_h.h_bsize
		);
		ret++;
	}

	/*
	 *	Compara as SYMTBs
	 */
	if (old_h.h_ssize != new_h.h_ssize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das tabelas de simbolos diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_ssize, new_h.h_ssize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"SYMTB", old_h.h_ssize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_ssize;
	new_off += new_h.h_ssize;

	/*
	 *	Compara relocação do TEXT
	 */
	if (old_h.h_rtsize != new_h.h_rtsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das relocações de TEXT diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_rtsize, new_h.h_rtsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"relocação do TEXT", old_h.h_rtsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_rtsize;
	new_off += new_h.h_rtsize;

	/*
	 *	Compara relocação do DATA
	 */
	if (old_h.h_rdsize != new_h.h_rdsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das relocações de DATA diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_rdsize, new_h.h_rdsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"relocação do DATA", old_h.h_rdsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_rdsize;
	new_off += new_h.h_rdsize;

	/*
	 *	Compara as tabela de linhas
	 */
	if (old_h.h_lnosize != new_h.h_lnosize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das tabelas de linhas diferem%s:  (%d :: %d)\n",
			warn_color_str, def_color_str, old_h.h_lnosize, new_h.h_lnosize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de linhas", old_h.h_lnosize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_lnosize;
	new_off += new_h.h_lnosize;

	/*
	 *	Compara as tabela do depurador
	 */
	if (old_h.h_dbsize != new_h.h_dbsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das tabelas do depurador diferem%s:  (%d :: %d)\n",
			warn_color_str, def_color_str, old_h.h_dbsize, new_h.h_dbsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela do depurador", old_h.h_dbsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_dbsize;
	new_off += new_h.h_dbsize;

	/*
	 *	Compara tabela de módulos
	 */
	if (old_h.h_modsize != new_h.h_modsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das tabela de módulos diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_modsize, new_h.h_modsize
		);

		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de módulos", old_h.h_modsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	old_off += old_h.h_modsize;
	new_off += new_h.h_modsize;

	/*
	 *	Compara as tabelas de referências externas
	 */
	if (old_h.h_refsize != new_h.h_refsize)
	{
		fprintf
		(	stderr,
			"\t%sOs tamanhos das tabelas de referências externas diferem%s:  (%d :: %d)\n",
			error_color_str, def_color_str, old_h.h_refsize, new_h.h_refsize
		);
		ret++;
	}
	else
	{
		ret += section_cmp
		(	"tabela de referências externas", old_h.h_refsize,
			old_fp, new_fp, old_off, new_off
		);
	}

	fclose (old_fp);
	fclose (new_fp);

	if (ret)
	{
		fprintf
		(	stderr,
		"	Tamanhos totais: (%d :: %d)\n",
			old_h.h_tsize + old_h.h_dsize + old_h.h_bsize,
			new_h.h_tsize + new_h.h_dsize + new_h.h_bsize
		);

		data_cmp (old_nm, new_nm, &old_h, &new_h);
		return (1);
	}

	return (0);

}	/* end compare */

/*
 ****************************************************************
 *	Identifica, obtém o estado e abre arquivos		*
 ****************************************************************
 */
int
praeludium (int index, const char **file_nm, STAT *sp, FILE **fp, HEADER *hp)
{
	off_t		total = 0;
	int		n, errno_code = 0;

	/*
	 *	Obtém o estado do arquivo
	 */
	if (stat (*file_nm, sp) < 0)
		{ sp->st_mode = 0; errno_code = errno; }

	/*
	 *	Imprime o nome do arquivo, se for o caso
	 */
	if (index == 0 && dirend)
	{
		fprintf
		(	stderr,
			"%s%s%s:\n",
			color_vector[(sp->st_mode & S_IFMT) >> 12],
			*file_nm,
			color_vector[MC_DEF]
		);
	}

	if (sp->st_mode == 0)
	{
		fprintf
		(	stderr,
			"\t%sNão consegui obter o estado de \"%s\"%s (%s)\n",
			error_color_str, *file_nm, def_color_str, strerror (errno_code)
		);

		return (1);
	}

	/*
	 *	Verifica se é um arquivo regular
	 */
	if (!S_ISREG (sp->st_mode))
	{
		fprintf
		(	stderr,
			"\t%s\"%s\" não é um arquivo regular%s\n",
			error_color_str, *file_nm, def_color_str
		);
		return (1);
	}

	/*
	 *	Abre o arquivo
	 */
	if ((*fp = fopen (*file_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"\t%sNão consegui abrir \"%s\"%s (%s)\n",
			error_color_str, *file_nm, def_color_str, strerror (errno)
		);
		return (1);
	}

	n = fread (hp, sizeof (HEADER), 1, *fp);

	if (n != 1 || hp->h_magic != FMAGIC && hp->h_magic != NMAGIC && hp->h_magic != SMAGIC)
	{
		fprintf
		(	stderr,
			"\t%s\"%s\" não é um módulo objeto%s\n",
			error_color_str, *file_nm, def_color_str
		);
		fclose (*fp);
		return (1);
	}

	/*
	 *	Verifica se o cabeçalho é válido
	 */
	if (hp->h_tsize < 0 || hp->h_dsize < 0  || hp->h_bsize < 0)
		total = -1;

	if (hp->h_ssize < 0 || hp->h_rtsize < 0 || hp->h_rdsize < 0)
		total = -1;

	if (total == 0)
		total = hp->h_tsize + hp->h_dsize + hp->h_ssize + hp->h_rtsize + hp->h_rdsize;

	if (total >= 0 && total < sp->st_size)
		return (0);

	fprintf
	(	stderr,
		"\t%sO módulo objeto \"%s\" tem cabeçalho inválido%s\n",
		error_color_str, *file_nm, def_color_str
	);
	fclose (*fp);
	return (1);

}	/* end praeludium */

/*
 ****************************************************************
 *	Compara uma seção					*
 ****************************************************************
 */
int
section_cmp (const char *sectionm, off_t n, FILE *old_fp, FILE *new_fp, off_t old_off,  off_t new_off)
{
	int		i;

	fseek (old_fp, old_off, SEEK_SET);
	fseek (new_fp, new_off, SEEK_SET);

	for (i = 0; i < n; i++)
	{
		if (getc (old_fp) != getc (new_fp))
		{
			fprintf
			(	stderr,
				"\t%sDiferença na seção \"%s\"%s no byte %d\n",
				error_color_str, sectionm, def_color_str, i
			);

			return (1);
		}
	}

	return (0);

}	/* end section_cmp */

/*
 ****************************************************************
 *	Verifica qual é mais recente				*
 ****************************************************************
 */
void
data_cmp (const char *old_nm, const char *new_nm, HEADER *old_hp,  HEADER *new_hp)
{
	if   (old_hp->h_time > new_hp->h_time)
		fprintf (stderr, "\tO mais recente é \"%s\"\n", old_nm);
	elif (old_hp->h_time < new_hp->h_time)
		fprintf (stderr, "\tO mais recente é \"%s\"\n", new_nm);
	else
		fprintf (stderr, "\tOs módulos objeto têm a mesma data\n");

}	/* end data_cmp */

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
		"%s - compara pares de módulos objeto\n"
		"\n%s\n",
		pgname, pgversion
	);

	fprintf
	(	stderr,
		"\nSintaxe:\n"
		"\t%s [-q] <módulo1> <módulo2>\n"
		"\t%s [-q] <módulo1> [<módulo2> ...] <diretório>\n"
		"\t%s [-q] [-N] <diretório>\n",
		pgname,	pgname,	pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-q: Comparação rápida\n"
		"\t-N: Lê os nomes dos <módulo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se for dado apenas o <diretório>\n"
	);

	exit (2);

}	/* end help */
