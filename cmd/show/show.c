/*
 ****************************************************************
 *								*
 *			show.c					*
 *								*
 *	Mostra um arquivo na tela do terminal			*
 *								*
 *	Versão	1.0.0, de 01.04.87				*
 *		4.7.0, de 28.12.04				*
 *								*
 *	Módulo: show						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <termio.h>
#include <a.out.h>
#include <signal.h>
#include <terminfo.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.7.0, de 28.12.04";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

const char	dev_tty_nm[] =  "/dev/tty";

entry int	wflag;		/* Dobra linhas compridas */
entry int	vflag;		/* Verbose */
entry int	Dflag;		/* Lista de manuais */

enum { MAN, MODIF, SHOW }	function;

entry const char *man_dir;	/* Nome do diretório dos manuais */
entry const char *file_nm;	/* Nome do manual procurado */
entry const char *dir_nm;	/* Nome do diretório onde foi encontrado */

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	HEADER_ENDIAN_CV
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	HEADER_ENDIAN_CV

/*
 ****** Caracteres de controle **********************************
 */
#define		CTLA	0x01
#define		CTLB	0x02
#define		CTLC	0x03
#define		CTLD	0x04
#define		CTLE	0x05
#define		CTLF	0x06
#define		CTLG	0x07
#define		CTLI	0x09
#define		CTLL	0x0C
#define		CTLQ	0x11
#define		CTLR	0x12
#define		CTLS	0x13
#define		CTLU	0x15
#define		CTLV	0x16
#define		CTLY	0x19
#define		CTLZ	0x1A

/*
 ****** Estrutura da linha **************************************
 */
typedef	struct	fline	FLINE;

struct	fline
{
	FLINE	*l_back;	/* Ponteiro para a linha anterior */
	FLINE	*l_forw;	/* Ponteiro para a linha seguinte */
	char	*l_text;	/* Texto da linha */
	int	l_len;		/* Tamanho da linha */
	int	l_index;	/* Número da linha */
};	

entry int	NLINE;		/* Número de linhas */
entry int	NCOL;		/* Número de colunas */

#define	NOFLINE	(FLINE *)NULL

entry	FLINE	*file_first_line; /* Ponteiro para a primeira linha */
entry	FLINE	*file_last_line;  /* Ponteiro para a última   linha */

/*
 ****** Dados de "terminfo" *************************************
 */
entry INFO	info;		/* Informações sobre o Terminal */

entry const char *c_clear,	/* Limpa a tela */
		*c_cup,		/* Endereça o cursor */
		*c_ed,		/* Limpa até o resto da tela */
		*c_el,		/* Limpa até o resto da linha */
		*c_ri,		/* Scroll reverso */
		*c_rev,		/* Reverse video */
		*c_sgr0;	/* Termina um campo */

entry TERMIO	new_termio, old_termio; /* Estado do terminal */

entry int	window_changed; /* Foi alterado o tamanho da janela */

/*
 ****** Variáveis globais ***************************************
 */
entry FILE	*fp_file;	/* Arquivo de entrada */
entry FILE	*fp_tty;	/* Teclado */
entry FILE	*fp_out;	/* Tela do terminal */

entry int	file_is_stdin;	/* O arquivo foi lido da entrada padrão */

entry int	file_size;	/* Tamanho do arquivo */
entry int	file_line_cnt;	/* No. de linhas do arquivo */
entry int	file_page_cnt;	/* No. de páginas do arquivo */

entry int	long_line_cnt;	/* Contador de linhas longas */

/*
 ****** Variáveis referentes à tela *****************************
 */
entry int	TEXTLINE;	/* Última linha de texto (começando de 0) */
entry int	LASTLINE;	/* Linha da mensagem (começando de 0) */

#define	PAGESZ	75		/* No. de linhas por página de manual */

#define	LINETOPG(i)	((i + PAGESZ - 1)/PAGESZ)

entry	FLINE	*screen_first_line; /* Ponteiro para a primeira linha */
entry	FLINE	*screen_last_line;  /* Ponteiro para a última   linha */

/*
 ******	Elemento da lista de nomes de manuais *******************
 */
typedef struct	info	DINFO;

struct	info
{
	char	i_nm[16];	/* Nome de manual */
	DINFO	*i_next;	/* Próximo elemento da lista */
};

#define	NODINFO	(DINFO *)NULL

entry DINFO	filelist;	/* Cabeça da lista de arquivos */

/*
 ****** Protótipos de funções ***********************************
 */
FILE		*search_directory_list (int code);
FILE		*search_one_directory (const char *dir);
void		read_file (FILE *);
void		make_directory_list (int code);
void		list_insert (DINFO *member);
void		insert_directory_line (char *area);
void		execute_cmd (void);
void		write_one_line (const FLINE *);
void 		write_file_info (void);
void		on_signal (int, ...);
void		on_winch_signal (int signo, ...);
void		reset_tty (void);
void		help (void);

/*
 ****************************************************************
 *	Mostra um arquivo na tela do terminal			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, code = 0;
	TERMIO		*tp;
	const char	*str;
	STAT		s;

	/*
	 *	Abre o terminal para leituras dos comandos
	 */
	if ((fp_tty = fopen (dev_tty_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", dev_tty_nm);

	/*
	 *	Abre o terminal para a saída na tela
	 */
	if ((fp_out = fopen (dev_tty_nm, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", dev_tty_nm);

	/*
	 *	Obtém o TERMIO do terminal
	 */
	if (ioctl (fileno (fp_tty), TCGETS, tp = &old_termio) < 0)
		error ("$*Não consegui obter o \"termio\" de \"%s\"", dev_tty_nm);

	/*
	 *	Calcula o número de linhas e colunas
	 */
	if (tp->t_nline == 0)
		tp->t_nline = 24;

	if (tp->t_ncol == 0)
		tp->t_ncol = 80;

	TEXTLINE = tp->t_nline - 3;
	LASTLINE = tp->t_nline - 1;

	NLINE	 = tp->t_nline;
	NCOL	 = tp->t_ncol;

	/*
	 *	Investiga qual o papel ele deve interpretar
	 */
	str = "cslfwnxrge123DdvMH";

	if   (patmatch (pgname, "*man"))
		function = MAN;
	elif (patmatch (pgname, "*modif"))
		function = MODIF;
	else
		{ function = SHOW; str = "dvMH"; }

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, str)) != EOF)
	{
		switch (opt)
		{
		    case 'c':			/* Comandos */
		    case '1':
			code = 'c';
			break;

		    case 's':			/* Chamadas ao sistema */
		    case '2':
			code = 's';
			break;

		    case 'l':			/* Biblioteca */
		    case '3':
			code = 'l';
			break;

		    case 'f':			/* Formatos */
			code = 'f';
			break;

		    case 'w':			/* Curses (window) */
			code = 'w';
			break;

		    case 'n':			/* Network (xti) */
			code = 'n';
			break;

		    case 'x':			/* Xwindow */
			code = 'x';
			break;

		    case 'r':			/* Guias */
		    case 'g':
			code = 'r';
			break;

		    case 'e':			/* etc ... */
			code = 'e';
			break;

		    case 'D':			/* Lista de manuais */
			Dflag++;
			break;

		    case 'd':			/* Dobra linhas compridas */
			wflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'M':			/* Memory */
			exit (0);

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (Dflag && argc)
		help ();

	/*
	 *	Verifica se foi dado mais de um nome de arquivo
	 */
	if (argc > 1)
	{
		int	i;
		char		int_buf[8];

		for (EVER)
		{
			fprintf
			(	stderr,
				"\nForam dados %d arquivos:\n\n",
				argc
			);

			for (i = 0; i < argc; i++)
				fprintf (stderr, "%2d: %s\n", i + 1, argv[i]);

			fprintf
			(	stderr,
				"\nIndique qual o desejado (1 a %d): ",
				argc
			);

			if ((i = atol (gets (int_buf))) > 0 && i <= argc)
			{
				argv[0] = argv[i-1];
				break;
			}

		}	/* end for (EVER) */

	}	/* end if (dado mais de um arquivo) */

	file_nm = argv[0];

	/*
	 *	Lê o arquivo adequado
	 */
	switch (function)
	{
	    case SHOW:
		if (file_nm == NOSTR)
		{
			if (isatty (0))		/* Se na realidade NÃO é um PIPE */
				exit (0);

			fp_file = stdin;
			file_nm = "stdin";
			file_is_stdin++;
		}
		else
		{
			if ((fp_file = fopen (file_nm, "r")) == NOFILE)
				error ("$*Não consegui abrir \"%s\"", file_nm);

			if (fstat (fileno (fp_file), &s) < 0) 
				error ("$*Não consegui obter o estado de \"%s\"", file_nm);

			if (!S_ISREG (s.st_mode))
				error ("$\"%s\" não é um arquivo regular", file_nm);	
		}

		read_file (fp_file);

		break;

	    case MAN:
	    case MODIF:
		if (Dflag)
		{
			if ((man_dir = getenv ("mandir")) == NOSTR)
				man_dir = "/usr/man";

			make_directory_list (code);

			file_nm = alloca (80);

			snprintf ((char *)file_nm, 80, "Índice da seção \"%s\"", dir_nm);
		}
		else
		{
			if (file_nm == NOSTR)
				return (0);

			/* Verifica se é uma execução "dummy" */

			if ((man_dir = getenv ("man_exec")) != NOSTR && streq (man_dir, "DUMMY"))
				exit (0);

			/* Procura o nome do diretório raiz dos manuais	*/

			if ((man_dir = getenv ("mandir")) == NOSTR)
				man_dir = "/usr/man";

			if ((fp_file = search_directory_list (code)) == NOFILE)
				error ("$Não encontrei informações sobre \"%s\"", file_nm);

			read_file (fp_file);
		}

		break;

	}	/* end switch */


#ifdef	DEBUG
	printf ("Primeira  linha = \"%s\"\n", file_first_line->l_text);
	printf ("Segunda   linha = \"%s\"\n", file_first_line->l_forw->l_text);
	printf ("Penúltima linha = \"%s\"\n", file_last_line->l_back->l_text);
	printf ("Última    linha = \"%s\"\n", file_last_line->l_text);
#endif	DEBUG

	/*
	 *	Obtem as características do terminal
	 */
	if (getinfo (NOSTR, &info) == 0)
		exit (1);

	/*
	 *	Obtém ponteiros para as cadeias relevantes de "info"
	 */
	if ((c_clear = info.i_strings[s_clear]) == NOSTR)
		error ("$O terminal não pode limpar a tela?");

	if ((c_cup = info.i_strings[s_cup]) == NOSTR)
		error ("$O terminal não pode endereçar o cursor?");

	if ((c_ed = info.i_strings[s_ed]) == NOSTR)
		error ("$O terminal não pode limpar até o resto da janela?");

	if ((c_el = info.i_strings[s_el]) == NOSTR)
		error ("$O terminal não pode limpar até o resto da linha?");

	if ((c_ri = info.i_strings[s_ri]) == NOSTR)
		error ("$O terminal não pode executar um \"scroll\" reverso?");

	if   ((c_rev = info.i_strings[s_rev]) == NOSTR)
		{ c_rev = ""; c_sgr0 = ""; 	}
	elif ((c_sgr0 = info.i_strings[s_sgr0]) == NOSTR)
		{ c_rev = ""; c_sgr0 = ""; 	}

	/*
	 *	Põe a entrada do terminal em RAW
	 */
	ioctl (fileno (fp_tty), TCGETS, &old_termio);

	tp = &new_termio; *tp = old_termio;

	tp->c_oflag &= ~OPOST;
	tp->c_lflag &= ~(ISIG|ICANON|ECHO);
	tp->t_min = 1;

	if (ioctl (fileno (fp_tty), TCSETNR, tp) < 0)
		error ("$*Não consegui modificar o \"termio\" de \"%s\"", dev_tty_nm);

	signal (SIGTERM,   on_signal);
	signal (SIGINT,    on_signal);
	signal (SIGWINCH,  on_winch_signal);

	atexit (reset_tty);

	/*
	 *	Executa os comandos
	 */
	execute_cmd ();

	return (0);

}	/* end show */

/*
 ****************************************************************
 *	Percorre os vários diretórios				*
 ****************************************************************
 */
FILE *
search_directory_list (int code)
{
	FILE		*fp;

	/*
	 *	Na busca, repare que os casos recaem nos seguintes
	 */
	switch (code)
	{
	    default:
	    case 'c':
		if ((fp = search_one_directory ("cmd")) || code == 'c')
			break;
	    case 's':
		if ((fp = search_one_directory ("sys")) || code == 's')
			break;
	    case 'l':
		if ((fp = search_one_directory ("libc")) || code == 'l')
			break;
	    case 'f':
		if ((fp = search_one_directory ("fmt")) || code == 'f')
			break;
	    case 'w':
		if ((fp = search_one_directory ("curses")) || code == 'w')
			break;
	    case 'n':
		if ((fp = search_one_directory ("xti")) || code == 'n')
			break;
	    case 'x':
		if ((fp = search_one_directory ("xwin")) || code == 'x')
			break;
	    case 'r':
		if ((fp = search_one_directory ("ref"))  || code == 'r')
			break;
	    case 'e':
		fp = search_one_directory ("etc");
		break;

	}	/* end switch */

	return (fp);

}	/* end search_directory_list */

/*
 ****************************************************************
 *	Procura o manual no diretório especificado		*
 ****************************************************************
 */
FILE *
search_one_directory (const char *dir)
{
	FILE		*fp;
	char		*cp;

	cp = alloca (strlen (man_dir) + strlen (dir) + strlen (file_nm) + 3 + 6);

	strcpy (cp, man_dir); 	strcat (cp, function == MAN ?  "/" : "/modif/");
	strcat (cp, dir); 	strcat (cp, "/");
	strcat (cp, file_nm);

	if ((fp = fopen (cp, "r")) == NOFILE)
		return (NOFILE);

	dir_nm = dir;

	return (fp);

}	/* end search_one_directory */

/*
 ****************************************************************
 *	Le o arquivo para a memória				*
 ****************************************************************
 */
void
read_file (FILE *fp_in)
{
	char		*cp, *end_cp;
	FLINE		*lp;
	int		c, len, cnt = 1;
	char		area[1024];
	long		magic;

	/*
	 *	Verifica se é um módulo objeto
	 */
	if (!file_is_stdin)
	{
		HEADER		h;

		if ((fread (&h, sizeof (HEADER), 1, fp_in)) == 1)
		{
			magic = ENDIAN_SHORT (h.h_magic);

			if (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC)
				error ("$\"%s\" é um módulo objeto\r", file_nm);
		}

		rewind (fp_in);
	}

	/*
	 *	Malha lendo todos os caracteres do arquivo
	 */
	end_cp = &area[sizeof (area)];

	for (EVER)
	{
		/*
		 *	Testa o final do arquivo (após um <nl>)
		 */
		if ((c = getc (fp_in)) == EOF)
			break;

		/*
		 *	Lê uma linha
		 */
		for (cp = &area[0], len = 0; c != '\n' && c != EOF; c = getc (fp_in))
		{
			file_size++;

			if   (c == '\b')
			{
				if (cp > area)
					{ cp--; len--; }
			}
			elif (c == '\r')
			{
				if ((c = getc (fp_in)) == EOF)
					break;

				if (c == '\n')
					break;

				ungetc (c, fp_in); cp = &area[0]; len = 0; file_size--;
			}
			elif (c == '\t')
			{
				do
				{
					if (cp < end_cp)
						*cp++ = ' ';
				}
				while ((++len & 7) != 0);
			}
			elif (c >= CTLA  &&  c <= CTLZ)
			{
				if (cp + 1 < end_cp)
					{ *cp++ = '^'; *cp++ = c|0x40; len += 2; }
			}
			elif (cp < end_cp)
			{
				*cp++ = c; len++;
			}

			/*
			 *	Verifica se deve "dobrar" a linha
			 */
			if (len >= NCOL && wflag)	/* dobra */
			{
				if ((c = getc (fp_in)) != '\n' && c != EOF)
				{
					long_line_cnt++;
					file_size--; file_line_cnt--;
					ungetc (c, fp_in);
				}

				break;

			}	/* end if (len >= NCOL) */

		}	/* end while not <nl> nor EOF */

		*cp = '\0';

		/*
		 *	Cria o bloco para uma linha
		 */
		if ((lp = malloc (sizeof (FLINE))) == NOFLINE)
			error (NOSTR);

		if ((lp->l_text = malloc (len + 1)) == NOSTR)
			error (NOSTR);

		strcpy (lp->l_text, area);
		lp->l_len	= len;
		lp->l_index	= cnt++;

		/*
		 *	Atualiza a lista duplamente encadeada
		 */
		if (file_first_line == NOFLINE)
			file_first_line = lp;

		lp->l_back = file_last_line;
		lp->l_forw = NOFLINE;

		if (file_last_line != NOFLINE)
			file_last_line->l_forw = lp;

		file_last_line = lp;

		/*
		 *	Testa o final do arquivo (linha sem <nl>)
		 */
		if (c == EOF)
			break;
		else
			{ file_size++; file_line_cnt++; }

	}	/* end FOR EVER */

	/*
	 *	Não esquece de verificar se houve êrro de leitura
	 */
	if (ferror (fp_in))
		error ("$*Erro de leitura de \"%s\"", file_nm);

	file_page_cnt = LINETOPG (file_line_cnt);

}	/* end read_file */

/*
 ****************************************************************
 *	Constrói uma lista dos manuais da seção dada		*
 ****************************************************************
 */
void
make_directory_list (int code)
{
	int		i;
	DIR		*dir_fp;
	DINFO		*ip;
	char		*cp, *dir;
	char		*area = NOSTR;
	const DIRENT	*dp;

	/*
	 *	Obtém o nome do diretório pedido
	 */
	switch (code)
	{
	    default:
	    case 'c':
		dir = "cmd";
		break;

	    case 's':
		dir = "sys";
		break;

	    case 'l':
		dir = "libc";
		break;

	    case 'f':
		dir = "fmt";
		break;

	    case 'w':
		dir = "curses";
		break;

	    case 'n':
		dir = "xti";
		break;

	    case 'x':
		dir = "xwin";
		break;

	    case 'r':
		dir = "ref";
		break;

	    case 'e':
		dir = "etc";
		break;

	}	/* end switch */

	/*
	 *	Prepara o nome do diretório e o abre
	 */
	cp = alloca (strlen (man_dir) + strlen (dir) + 2 + 6);

	strcpy (cp, man_dir); 	strcat (cp, function == MAN ?  "/" : "/modif/");
	strcat (cp, dir);

	if ((dir_fp = opendir (cp)) == NODIR)
		error ("$*Não consegui ler o diretório \"%s\"", cp);	

	dir_nm = dir;

	/*
	 *	Le o diretório
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		if ((ip = malloc (sizeof (DINFO))) == NODINFO)
			error (NOSTR);

		strscpy (ip->i_nm, dp->d_name, sizeof (ip->i_nm));

		list_insert (ip);

	}	/* end while (fread) */

	closedir (dir_fp);

	/*
	 *	Coloca o título
	 */
	if ((cp = malloc (NCOL + 1)) == NOSTR)
		error (NOSTR);

	snprintf
	(	cp, NCOL + 1,
		"Manuais%sdisponíveis na seção \"%s\":",
		function == MODIF ? " de modificações " : " ", dir
	); 

	insert_directory_line (cp);  	insert_directory_line ("");

	/*
	 *	Prepara a as linhas (já ordenado alfabeticamente)
	 */
	for (i = 0, ip = filelist.i_next; ip != NODINFO; ip = ip->i_next)
	{
		if (i >= 5)
			{ *cp = '\0'; insert_directory_line (area); i = 0; }

		if (i == 0)
		{
			if ((area = cp = malloc (NCOL + 1)) == NOSTR)
				error (NOSTR);

			memset (cp, ' ', NCOL);	cp += 2;
		}

		memcpy (cp, ip->i_nm, strlen (ip->i_nm));

		i++;	cp += IDSZ + 1;

	}	/* end for (lendo o diretório) */

	if (i > 0)
		{ *cp = '\0'; insert_directory_line (area); }

	file_page_cnt = LINETOPG (file_line_cnt);

}	/* end make_directory_list */

/*
 ****************************************************************
 *	Insere no ponto correto da lista			*
 ****************************************************************
 */
void
list_insert (DINFO *member)
{
	DINFO		*ip;
	char		*nm;

	/*
	 *	Insere alfabeticamente
	 */
	nm = member->i_nm;

	for (ip = &filelist; ip->i_next != NODINFO; ip = ip->i_next)
	{
#define	FOLD
#ifdef	FOLD
		if (strttcmp (nm, ip->i_next->i_nm, cmpfoldtb, cmpafoldtb) <= 0)
#else
		if (strcmp (nm, ip->i_next->i_nm) <= 0)
#endif	FOLD
			break;
	}

	member->i_next = ip->i_next;
	ip->i_next = member;

#if (0)	/*******************************************************/
	section_man_cnt++;
#endif	/*******************************************************/

}	/* end list_insert */

/*
 ****************************************************************
 *	Insere uma linha do diretório de manuais		*
 ****************************************************************
 */
void
insert_directory_line (char *area)
{
	FLINE		*lp;

	/*
	 *	Cria o bloco para uma linha
	 */
	if ((lp = malloc (sizeof (FLINE))) == NOFLINE)
		error (NOSTR);

	lp->l_text	= area;
	lp->l_len	= strlen (area);
	lp->l_index	= ++file_line_cnt;

	file_size += lp->l_len;

	/*
	 *	Atualiza a lista duplamente encadeada
	 */
	if (file_first_line == NOFLINE)
		file_first_line = lp;

	lp->l_back = file_last_line;
	lp->l_forw = NOFLINE;

	if (file_last_line != NOFLINE)
		file_last_line->l_forw = lp;

	file_last_line = lp;

}	/* end insert_directory_line */

/*
 ****************************************************************
 *	Executa os comandos					*
 ****************************************************************
 */
void
execute_cmd (void)
{
	int		cmd, c, i, n = 0, msg = 0;
	char		*cp;
	FLINE		*lp;
	FILE		*fp;
	char		read_area[80], pattern_area[80];

	/*
	 *	Verifica se de fato há texto
	 */	 
	if ((lp = file_first_line) == NOFLINE)
		exit (0);

	pattern_area[0] = '\0';

	/*
	 *	Lista uma tela
	 */	 
	fputs (c_clear, fp_out);

	screen_first_line = lp;

	for (i = TEXTLINE; i >= 0 && lp != NOFLINE; i--, lp = lp->l_forw)
	{
		write_one_line (lp);
		screen_last_line = lp;
	}

	/*
	 *	Põe logo o nome do arquivo
	 */
	write_file_info ();
	fflush (fp_out);
	msg++;

	/*
	 *	Malha de leitura e executação dos comandos
	 */
	while ((cmd = getc (fp_tty)) != CTLC  &&  cmd != 'q'  &&  cmd != 'Q')
	{
		/*
		 *	Verifica se mudou o tamanho da janela
		 */
		if (window_changed)
		{
			window_changed = 0;
			lp = screen_first_line;
			goto clear_and_write_page;
		}

		/*
		 *	Analisa o comando (antes de tirar a mensagem)
		 */
 		switch  (cmd)
		{
			/*
			 *	CSTOP & CSTART
			 */
		    case CTLQ:
		    case CTLS:
			continue;

		}	/* end switch */

		/*
		 *	Retira a mensagem do final da tela
		 */
		if (msg)
		{
			fprintf (fp_out, "\r%s", c_ed);
			msg = 0;
		}

		/*
		 *	Analisa o comando (depois de tirar a mensagem)
		 */
 		switch  (cmd)
		{
			/*
			 *	Avança <n> linhas
			 */
		    case '\n':
		    case '\r':
		    case '+':
		    case 'j':
		    case CTLE:
		    case_plus_line:
			if (n > (TEXTLINE + 1)/2)
				{ n += screen_first_line->l_index; goto case_g; }

			i = screen_last_line->l_index - screen_first_line->l_index;

			lp = screen_last_line->l_forw;

			if (i == 0)
				{ putc ('\a', fp_out); n = 0; break; }

			do
			{
				if (i == 0)
					break;

				fprintf (fp_out, "\n");
				screen_first_line = screen_first_line->l_forw;

				if (lp != NOFLINE)
				{
					fprintf
					(	fp_out, "%s",
						parmexec (c_cup, TEXTLINE, 0)
					);
					write_one_line (lp);
					screen_last_line  = lp;
					fprintf
					(	fp_out, "%s",
						parmexec (c_cup, LASTLINE, 0)
					);
					lp = lp->l_forw;
				}
				else
				{
					i--;
				}

			}		
			while (--n > 0);

			n = 0;
			break;				

			/*
			 *	Retrocede <n> linhas
			 */
		    case '-':
		    case '=':
		    case 0x1D:
		    case 'k':
		    case CTLY:
		    case_minus_line:
			if ((lp = screen_first_line->l_back) == NOFLINE)
				{ putc ('\a', fp_out); n = 0; break; }

			if (n > (TEXTLINE + 1)/2)
				{ n = screen_first_line->l_index - n; goto case_g; }

			i = screen_last_line->l_index - screen_first_line->l_index;

			do
			{
				fprintf
				(	fp_out, "%s%s",
					parmexec (c_cup, 0, 0), c_ri
				);
				write_one_line (lp);
				screen_first_line = lp;

				if (i < TEXTLINE)
				{
					i++;
				}
				else
				{
					fprintf
					(	fp_out, "%s%s",
						parmexec (c_cup, i + 1, 0), c_el
					);
					screen_last_line  = screen_last_line->l_back;
				}

				if ((lp = lp->l_back) == NOFLINE)
					break;
			}		
			while (--n > 0);

			fprintf (fp_out, "%s", parmexec (c_cup, LASTLINE, 0));

			n = 0;
			break;

			/*
			 *	Avança meia página
			 */
		    case CTLD:
		    case_CTLD:
			i = file_last_line->l_index - screen_last_line->l_index;

			if (i <= 0)
				{ putc ('\a', fp_out); n = 0; break; }

			n = (TEXTLINE + 1)/2;

			if (n > i)
				n = i;

			goto case_plus_line;

			/*
			 *	Retrocede meia página
			 */
		    case CTLU:
		    case_CTLU:
			n = (TEXTLINE + 1)/2;

			goto case_minus_line;

			/*
			 *	Avança <n> telas
			 */
		    case CTLF:	
			i = (TEXTLINE + 1);
		    case_CTLF:	
			if ((lp = screen_last_line->l_forw) == NOFLINE)
				{ putc ('\a', fp_out); n = 0; break; }

			if (n == 0)
				n = i;
			else
				n *= i;

			lp = screen_first_line;

			for (i = n; lp->l_forw  && i > 0; i--)
				lp = lp->l_forw;

		    clear_and_write_page:
			screen_first_line = lp;

			fputs (c_clear, fp_out);

			for (i = TEXTLINE; i >= 0 && lp != NOFLINE; i--, lp = lp->l_forw)
			{
				write_one_line (lp);
				screen_last_line = lp;
			}

			n = 0;
			goto case_file_nm;

			/*
			 *	Retrocede <n> telas
			 */
		    case CTLB:
			i = (TEXTLINE + 1);
		    case_CTLB:
			if (screen_first_line->l_back == NOFLINE)
				{ putc ('\a', fp_out); n = 0; break; }

			if (n == 0)
				n = i;
			else
				n *= i;

			lp = screen_first_line;

			for (i = n; lp->l_back  && i > 0; i--)
				lp = lp->l_back;

			goto clear_and_write_page;

			/*
			 *	Avança <n> páginas
			 */
		    case CTLV:	
			i = PAGESZ;

			goto case_CTLF;

			/*
			 *	Retrocede <n> páginas
			 */
		    case CTLZ:
			i = PAGESZ;

			goto case_CTLB;

			/*
			 *	Reescreve a tela
			 */
		    case CTLR:
		    case_rewrite_screen:
			lp = screen_first_line;

			goto clear_and_write_page;

			/*
			 *	Escreve a linha contendo o nome do arquivo
			 */
		    case 'f':
		    case 'F':
		    case ':':
		    case CTLG:
		    case_file_nm:
			write_file_info ();
			msg++;
			n = 0;
			break;

			/*
			 *	Tira a mensagem da última linha da tela
			 */
		    case ' ':
			n = 0;
			break;

			/*
			 *	Posiciona dada a linha
			 */
		    case 'g':
		    case 'G':
		    case_g:
			if   (n == 0)			/* Última página */
			{
				lp = file_last_line;

				for (i = TEXTLINE; lp->l_back && i > 0; i--)
					lp = lp->l_back;
			}
			elif (n == 1)			/* Primeira página */
			{
				lp = file_first_line;
			}
			elif (wflag && long_line_cnt && (cmd == 'g' || cmd == 'G'))
			{
							/* Não corresponde */
				putc ('\a', fp_out); n = 0; break;
			}
			elif ((unsigned)n > file_line_cnt) /* Linha inválida */
			{
				putc ('\a', fp_out); n = 0; break;
			}
			else				/* Procura a linha */
			{
				for (lp = file_first_line; lp->l_forw; /* abaixo */)
				{
					if (lp->l_index == n)
						break;
					
					if ((lp = lp->l_forw) == NOFLINE)
					  { putc ('\a', fp_out); n = 0; break; }

				}
			}

			if (lp == screen_first_line)
				{ n = 0; break; }

		    scroll_or_clear:
			n = lp->l_index - screen_first_line->l_index;

			if (n <= (TEXTLINE + 1)/2  &&  n >= -(TEXTLINE + 1)/2)
			{
				/*
				 *	A Constante de linhas é crítica; se
				 *	(|n| > (TEXTLINE + 1)/2), entra em loop
				 */
				if (n > 0)
					{ goto case_plus_line; }
				else
					{ n = -n; goto case_minus_line; }
			}

			goto clear_and_write_page;

			/*
			 *	Posiciona dada a página
			 */
		    case 'p':
		    case 'P':
			if (n == 0)
				n = file_page_cnt;

			n = (n - 1) * PAGESZ + 1;

			goto case_g;

			/*
			 *	Um número
			 */
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			n = (n * 10) + (cmd - '0');
			break;

			/*
			 *	Cadeia de busca
			 */
		    case '/':
			n = 0;
			fprintf (fp_out, "%s/", parmexec (c_cup, LASTLINE - 1, 0));
			fflush (fp_out);

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			cp = pattern_area;
			while ((c = getc (fp_tty)) != '\n')
				*cp++ = c;
			*cp++ = '\0';
			ioctl (fileno (fp_tty), TCSETNR, &new_termio);
			fprintf (fp_out, "%s%s\n", parmexec (c_cup, LASTLINE - 1, 0), c_ed);

			for (cp = NOSTR, lp = file_first_line; lp; lp = lp->l_forw)
				if (cp = strstr (lp->l_text, pattern_area))
					break;

		    after_pattern_search:
			if (cp == NOSTR)
			{
				fprintf
				(	fp_out,
					"\a%s%sCadeia não encontrada: \"%s\"%s\r",
					parmexec (c_cup, LASTLINE, 0),
					c_rev, pattern_area, c_sgr0
				);
				msg++;
				break;
			}

			goto scroll_or_clear;

			/*
			 *	Repetição da cadeia de busca (adiante)
			 */
		    case 'n':
			n = 0;

			if (pattern_area[0] == '\0')
			{
			    missing_pattern:
				fprintf
				(	fp_out,
					"\a%s%sAinda não foi dada uma cadeia de busca%s\r",
					parmexec (c_cup, LASTLINE, 0), c_rev, c_sgr0
				);
				msg++;
				break;
			}

			for (cp = NOSTR, lp = screen_first_line->l_forw; lp; lp = lp->l_forw)
				if (cp = strstr (lp->l_text, pattern_area))
					break;

			goto after_pattern_search;

			/*
			 *	Repetição da cadeia de busca (volta)
			 */
		    case 'N':
			n = 0;

			if (pattern_area[0] == '\0')
				goto missing_pattern;

			for (cp = NOSTR, lp = screen_first_line->l_back; lp; lp = lp->l_back)
				if (cp = strstr (lp->l_text, pattern_area))
					break;

			goto after_pattern_search;

			/*
			 *	Salva o arquivo
			 */
		    case 's':
		    case 'S':
		    case 'w':
		    case 'W':
			n = 0;
			fprintf
			(	fp_out, "%s: %c ",
				parmexec (c_cup, LASTLINE - 1, 0), cmd
			);
			fflush (fp_out);

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			cp = read_area;
			while ((c = getc (fp_tty)) != '\n')
				*cp++ = c;
			*cp++ = '\0';
			ioctl (fileno (fp_tty), TCSETNR, &new_termio);
			fprintf
			(	fp_out, "%s%s\n",
				parmexec (c_cup, LASTLINE - 1, 0), c_ed
			);

			msg++;

			if (read_area[0] == '\0')
				break;

			fprintf (fp_out, "%s", c_rev);

			if ((cmd == 's' || cmd == 'w') && access (read_area, F_OK) >= 0)
			{	
				fprintf
				(	fp_out, "\a%s: "
					"O arquivo \"%s\" já existe",
					pgname, read_area
				);
				goto sgr0;
			}

			if ((fp = fopen (read_area, "w")) == NOFILE)
			{	
				fprintf
				(	fp_out, "\a%s: "
					"%s: Não consegui abrir \"%s\" (%s)",
					pgname, read_area, strerror (errno)
				);
				goto sgr0;
			}

			for (lp = file_first_line; lp; lp = lp->l_forw)
				fnputs (lp->l_text, fp);

			fflush (fp);

			if (ferror (fp))
			{
				fprintf
				(	fp_out, "\a%s: "
					"%s: Erro de escrita em \"%s\" (%s)",
					pgname, read_area, strerror (errno)
				);
				fclose (fp);
				goto sgr0;
			}

			fclose (fp);

			fprintf
			(	fp_out, "%s: "
				"Arquivo \"%s\" gravado",
				pgname, read_area
			);
			if (long_line_cnt) fprintf
			(	fp_out,
				wflag ? " (%d linhas dobradas)" : " (%d linhas truncadas)",
				long_line_cnt
			);

		    sgr0:
		   	fputs (c_sgr0, fp_out);
			break;

			/*
			 *	Invoca o "sh"
			 */
		    case '!':
		    case_sh:
			n = 0;
			fprintf (fp_out, "%s: !", parmexec (c_cup, LASTLINE - 1, 0));
			fflush (fp_out);

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			cp = read_area;
			while ((c = getc (fp_tty)) != '\n')
				*cp++ = c;
			*cp++ = '\0';

			if (read_area[0] == '\0')
			{
				ioctl (fileno (fp_tty), TCSETNR, &new_termio);
				fprintf
				(	fp_out, "%s%s\n",
					parmexec (c_cup, LASTLINE - 1, 0), c_ed
				);
				break;
			}

		    do_system:
			system (read_area);

			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

			fprintf (fp_out, "\r\nTecle para continuar");
			fflush (fp_out);
			while ((c = getc (fp_tty)) == CTLS || c == CTLQ)
				/* vazio */;

			if (c == '!')
			{
				fprintf
				(	fp_out, "%s%s\n",
					parmexec (c_cup, LASTLINE - 1, 0), c_ed
				);
				goto case_sh;
			}

			goto case_rewrite_screen;

			/*
			 *	Ponto de interrogação
			 */
		    case '?':
			n = 0;
			fprintf
			(	fp_out,
				"%s%sTecle <^D> e <^U> para ler o manual do comando \"%s\"%s",
				parmexec (c_cup, LASTLINE, 0), c_rev, pgname, c_sgr0
			);
			fflush (fp_out);

			while ((c = getc (fp_tty)) == CTLS || c == CTLQ)
				/* vazio */;

			snprintf (read_area, sizeof (read_area), "man %s", pgname);

			goto do_system;

			/*
			 *	Escape
			 */
		    case '\e':
			if (getc (fp_tty) != '[')
				goto case_default;

			switch (getc (fp_tty))
			{
			    case 'A':	/* Seta para cima */
				goto case_minus_line;

			    case 'B':	/* Seta para baixo */
				goto case_plus_line;

			    case '5':	/* Page up */
				goto case_CTLU;

			    case '6':	/* Page down */
				goto case_CTLD;

			    case '7':	/* Home */
				n = 1; goto case_g;

			    case '8':	/* End */
				n = 0; goto case_g;
			}

			goto case_default;

			/*
			 *	Til (ignora, final de seqüências de escape)
			 */
		    case '~':
			continue;

			/*
			 *	Caracter inválido
			 */
		    default:
		    case_default:
			fprintf
			(	fp_out, "%s%s\a%s: caracter inválido%s",
				parmexec (c_cup, LASTLINE, 0), c_rev,
				pgname, c_sgr0
			);
			msg++;
			n = 0;
			break;

		}	/* end switch */

		fputs (parmexec (c_cup, LASTLINE, 0), fp_out);
		fflush (fp_out);

	}	/* end while */

	fputs (parmexec (c_cup, LASTLINE, 0), fp_out);
	fputs (c_ed, fp_out);

}	/* end execute_cmd */

/*
 ****************************************************************
 *	Imprime uma linha					*
 ****************************************************************
 */
void
write_one_line (const FLINE *lp)
{
	int		len;
	const char	*cp, *end_cp;

	if ((len = lp->l_len) > NCOL)
		len = NCOL;

	for (cp = lp->l_text, end_cp = cp + len; cp < end_cp; cp++)
		putc (*cp, fp_out);

	if (len != NCOL)
		fprintf (fp_out, "\r\n");

}	/* end write_one_line */

/*
 ****************************************************************
 *	Imprime o nome do arquivo, ...				*
 ****************************************************************
 */
void
write_file_info (void)
{
	char		*area;

	area = alloca (NCOL);

	fprintf (fp_out, "%s%s", parmexec (c_cup, LASTLINE, 0), c_rev);

	snprintf
	(
		area, NCOL,
		"\"%s\": %d linhas, %d caracteres"
		" (linha %d), %dx%d",
		file_nm, file_line_cnt, file_size,
		screen_first_line->l_index,
		NCOL, NLINE
	);

	fputs (area, fp_out);
	fputs (c_sgr0, fp_out);

}	/* end write_file_info */

/*
 ****************************************************************
 *	Restaura o estado do terminal				*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	reset_tty ();
	exit (1);

}	/* end on_signal */

/*
 ****************************************************************
 *	Mudança do tamanho da janela				*
 ****************************************************************
 */
void
on_winch_signal (int signo, ...)
{
	TERMIO		termio;

	signal (SIGWINCH,  on_winch_signal);

	ioctl (fileno (fp_tty), TCGETS, &termio);

	TEXTLINE = termio.t_nline - 3;
	LASTLINE = termio.t_nline - 1;

	old_termio.t_nline = NLINE = termio.t_nline;
	old_termio.t_ncol  = NCOL  = termio.t_ncol;
	old_termio.t_page  = termio.t_page;

	window_changed++;

}	/* end on_signal */

/*
 ****************************************************************
 *	Restaura o estado do terminal				*
 ****************************************************************
 */
void
reset_tty (void)
{
	ioctl (fileno (fp_tty), TCSETNR, &old_termio);

}	/* end reset_tty */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	switch (function)
	{
	    case SHOW:
		fprintf
		(	stderr,
			"%s - mostra um arquivo em uma janela do vídeo\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-d] [<arquivo> ...]\n",
			pgname, pgversion, pgname
		);
		fprintf
		(	stderr,
			"\nOpções:"
			"\t-d: Dobra as linhas maiores do que %d caracteres\n"
			"\nObs.:\tSe o nome do <arquivo> for omitido, será lida a entrada padrão\n",
			NCOL
		);
		break;

	    case MAN:
		fprintf
		(	stderr,
			"%s - mostra um manual em uma janela do vídeo\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-cslfwnxrgeD] [<manual>]\n",
			pgname, pgversion, pgname
		);
		fprintf
		(	stderr,
			"\nOpções:"
			"\t-c: O manual de um comando\n"
			"\t-s: O manual de uma chamada ao sistema\n"
			"\t-l: O manual de uma função da biblioteca\n"
			"\t-f: O manual de uma descrição de formato\n"
			"\t-w: O manual de uma função da \"curses\"\n"
			"\t-n: O manual de uma função de \"network\" (XTI)\n"
			"\t-x: Um manual do sistema \"X-Window\"\n"
			"\t-r: O guia de um comando\n"
			"\t-g: O guia de um comando\n"
			"\t-e: Outros manuais diversos\n"
			"\t-D: Mostra os nomes dos manuais disponíveis em uma das seções\n"
			"\t    (deve ser dada também a opção indicando a seção desejada)\n"
			"\nObs.:\tSem nenhuma das opções acima, mostra o primeiro "
				"<manual> encontrado\n"
			"\tsegundo a ordem dada\n"
		);
		break;

	    case MODIF:
		fprintf
		(	stderr,
			"%s - mostra um manual de modificações em uma janela do vídeo\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-cslwnxD] [<manual de modificações>]\n",
			pgname, pgversion, pgname
		);
		fprintf
		(	stderr,
			"\nOpções:"
			"\t-c: O manual de modificações de um comando\n"
			"\t-s: O manual de modificações de uma chamada ao sistema\n"
			"\t-l: O manual de modificações de uma função da biblioteca\n"
			"\t-w: O manual de modificações de uma função da \"curses\"\n"
			"\t-n: O manual de modificações de uma função de \"network\" (XTI)\n"
			"\t-x: O manual de modificações do sistema \"X-Window\"\n"
			"\t-D: Mostra os nomes dos manuais de modificações disponíveis em uma das\n"
			"\t    seções (deve ser dada também a opção indicando a seção desejada)\n"
			"\nObs.:\tSem nenhuma opção, mostra o primeiro "
				"<manual de modificações>\n"
			"\tencontrado segundo a ordem dada acima\n"
		);
		break;

	}	/* end switch */

	exit (2);

}	/* end help */
