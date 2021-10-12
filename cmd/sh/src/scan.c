/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador Léxico					*
 *								*
 *	Versão	3.2.2, de 15.09.99				*
 *		4.5.0, de 12.11.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/common.h>
#include	<sys/scb.h>
#include	<sys/syscall.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<setjmp.h>

#include	"../h/common.h"
#include	"../h/sym.h"
#include	"../h/scan.h"
#include	"../h/categ.h"
#include	"../h/error.h"
#include	"../h/command.h"

#include	"../h/extern.h"
#include	"../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Exportadas					*
 ****************************************************************
 */
entry	FILE	*input_file;		/* Arquivo de entrada */

entry	char	*strbuf;		/* Buffer auxiliar para strings */

entry	char	*linebuf;		/* Buffer para leitura */
entry	int	linebufsz;		/* Tamanho corrente do buffer */
entry	char	*line;			/* Linha mais recente */
entry	char	*end_linebuf;		/* Final da linha */
entry	char	*nextp;			/* Próximo caractere a processar */

entry	TOKEN	token;			/* Estrutura de retorno */
entry	RDRC	rdrc;			/* Estrutura de redirecionamento */

entry	int	lineprinted;		/* A linha já foi impressa */
entry	int	nline;			/* Número da linha */

extern	SCB	scb;			/* System Control Block */

/*
 ****************************************************************
 *	Macros Locais						*
 ****************************************************************
 */
#define	RETURN(m)	return (token.t_major = (m))

/*
 ****************************************************************
 *	Protótipos de Funções Locais				*
 ****************************************************************
 */
int	read_line (FILE *, int);
int	increase_linebuf_sz (void);
MAJOR	get_string (void);
MAJOR	get_redir (int);
char	*get_variable_value (char **, int);
void	put_prompt (const char *);

/*
 ****************************************************************
 *	Reinicializa o Analisador Léxico			*
 ****************************************************************
 */
void
restart_scanner (void)
{
	/*
	 *	Redireciona o arquivo de entrada para o "stdin",
	 *	se necessário.
	 */
	if (input_file != stdin)
	{
		fclose (input_file);
		input_file = stdin;
	}

	/*
	 *	Aloca as áreas auxiliares, se ainda não estiverem alocadas.
	 */
	if (linebuf == NOSTR)
	{
		if ((linebuf = malloc (BUFSZ)) == NOSTR)
			error (NOSTR);

		linebufsz = BUFSZ;
	}

	if (strbuf == NOSTR)
	{
		if ((strbuf = malloc (BUFSZ)) == NOSTR)
			error (NOSTR);
	}

	/*
	 *	Inicializa os ponteiros de modo que a primeira chamada
	 *	a "get_token" provoque a leitura de uma nova linha.
	 */
	nextp       = linebuf;
	end_linebuf = nextp - 1;

	in_command  = nesting = 0;

}	/* end restart_scanner */

/*
 ****************************************************************
 *	Analisador Léxico - Módulo Príncipal			*
 ****************************************************************
 */
MAJOR
get_token (void)
{
	uchar		catindex;
	char		*lp, c;

	/*
	 *	Se chegou ao fim, lê a próxima linha.
	 */
	if (nextp > end_linebuf)
	{
		if (input_file == stdin && interact && current_prompt != NOSYMP)
			put_prompt (current_prompt->s_value);

		if (read_line (input_file, 1) < 0)
		{
			/* Veio EOF, mas nem sempre retorna EOT... */

			if (dflag || input_file != stdin)
				RETURN (EOT);

			error ("Use \"exit\" para sair de sessão");

			nextp = end_linebuf + 1;

			RETURN (EOL);
		}

		nextp       = line;
		lineprinted = 0;
	}

	for (EVER)
	{
		token.t_begin = token.t_end = lp = nextp++;

		/*
		 *	Analisa a categoria/índice do caractere.
		 */
		switch (CATEG (catindex = categtb[c = *lp]))
		{
		    case INV:
			sherror (SHORTMARK, "Caractere inválido: %X", c);
			lp++;
			break;

		    case QUOTE:
			if (c == '\\' && CATEG (categtb[*nextp]) == SEPAR)
				break;

			RETURN (get_string ());

		    case SEPAR:
			break;

		    case DIGIT:
			if ((c = *nextp) == '<' || c == '>')
				RETURN (get_redir (DIGIT));

			/* continua no caso abaixo */

		    case LETTER:
		    case OTHER:
			RETURN (get_string ());

			/*
			 *	Delimitadores unários
			 */
		    case USYM:
			switch (CINDEX (catindex))
			{
			    case 0:		/* # */
				end_linebuf  = nextp - 1;
				*end_linebuf = NOCHR;

				/* continua no caso abaixo */

			    case 1:				/* '\0' */
				RETURN (EOL);

			    case 2:				/* ( */
				RETURN (LP);

			    case 3:				/* ) */
				RETURN (RP);

			    case 4:				/* { */
				RETURN (LC);

			    case 5:				/* } */
				RETURN (RC);
			}

			/*
			 *	Delimitadores binários
			 */
		    case BSYM:
			switch (CINDEX (catindex))
			{
			    case 0:
				if (*nextp == '&')
				{
					token.t_end++; nextp++;
					RETURN (AND);		/* && */
				}

				RETURN (BG);			/* & */

			    case 1:
				if (*nextp == ';')
				{
					token.t_end++; nextp++;
					RETURN (SMSM);		/* ;; */
				}

				if (nesting)
					RETURN (EOL);

				RETURN (SM);			/* ; */

			    case 2:				/* < */
			    case 3:				/* > */
				RETURN (get_redir (BSYM));

			    case 4:
				if (*nextp == '|')
				{
					token.t_end++; nextp++;
					RETURN (OR);		/* || */
				}

				RETURN (PIPE);			/* | */
			}

		    default:
			sherror (LONGMARK, "Categoria inválida: %X", catindex);

		}	/* end switch (categ) */

	}	/* end for (analisando caracteres) */

}	/* end get_token */

/*
 ****************************************************************
 *	Lê uma linha do arquivo especificado			*
 ****************************************************************
 */
int
read_line (FILE *fp, int expanding)
{
	int		c, line_diff;
	char		*lp;
	char		*endline = linebuf + linebufsz;

	if (in_command)
	{
		if ((lp = end_linebuf) >= endline - 4)
		{
			if (increase_linebuf_sz () < 0)
				error (NOSTR);

			lp	= end_linebuf;
			endline	= linebuf + linebufsz;
		}

		*lp++ = ' '; *lp++ = ';'; *lp++ = ' ';
	}
	else
	{
		lp = linebuf;
	}

	line = lp;	line_diff = line - linebuf;

	/*
	 *	Lê os caracteres até um EOF ou <nl>.
	 */
#define	RAW_MODE
#ifdef	RAW_MODE
	while ((c = fnrgets (&linebuf, &linebufsz, &lp, fp)) != EOF)
	{
		if (c == '\n')
		{
			nline++;

			/* Trata as linhas de continuação */

			if (!expanding || lp[-1] != '\\')
				break;		/* Fim de linha */

			lp[-1] = ' ';
		}
	}

	endline = linebuf + linebufsz;
	line    = linebuf + line_diff;
#else
	while ((c = getc (fp)) != EOF)
	{
		if (c == '\n')
		{
			nline++;

			/* Trata as linhas de continuação */

			if (!expanding || lp[-1] != '\\')
				break;		/* Fim de linha */

			lp[-1] = ' ';
			continue;
		}

		if (lp >= endline)
		{
			int	lp_diff = lp - linebuf;

			if (increase_linebuf_sz () < 0)
				error (NOSTR);

			lp	= linebuf + lp_diff;
			endline	= linebuf + linebufsz;
		}

		*lp++ = c;
	}
#endif	RAW_MODE

	*lp = NOCHR;

	end_linebuf = lp;	/* Guarda o fim da linha */

	if (c == EOF && lp == line)
		return (-1);

	if (vflag == 1)
		fprintf (stderr, "%3d %s\n", nline, line);

	return (lp - line);

}	/* end read_line */

/*
 ****************************************************************
 *	Aumenta o tamanho da área de leitura			*
 ****************************************************************
 */
int
increase_linebuf_sz (void)
{
	char		*new_linebuf;
	int		new_linesz;

	new_linesz  = linebufsz + BUFSZ;

	if ((new_linebuf = realloc (linebuf, new_linesz)) == NOSTR)
		return (-1);

	end_linebuf = new_linebuf + (end_linebuf - linebuf);
	line	    = new_linebuf + (line        - linebuf);

	linebuf     = new_linebuf;
	linebufsz   = new_linesz;

	return (0);

}	/* end increase_linebuf_sz */

/*
 ****************************************************************
 *	Insere uma linha do buffer (na marra)			*
 ****************************************************************
 */
void
insert_line (const char *new_line)
{
	int		len;

	len = strlen (new_line);

	if (len + 1 > linebufsz)
	{
		linebufsz = len << 1;
		linebuf   = realloc (linebuf, linebufsz);

		if (linebuf == NOSTR)
			error (NOSTR);
	}

	memmove (linebuf, new_line, len + 1);

	nextp       = line = linebuf;
	end_linebuf = &linebuf[len];
	lineprinted = 0;

}	/* end insert_line */

/*
 ****************************************************************
 *	Ignora até o fim da linha				*
 ****************************************************************
 */
void
skip_line (void)
{
	nextp = end_linebuf;

	if (*nextp != NOCHR)
		error ("GRAVE: \"end_linebuf\" inválido");

	token.t_end = nextp - 1;

	nextp++;

}	/* end skip_line */

/*
 ****************************************************************
 *	Analisa uma cadeia					*
 ****************************************************************
 */
MAJOR
get_string (void)
{
	char		*lp, *bp, c, *tmp;
	uchar		categ, catindex;
	int		defvar, inquote, quoted;
	SYM		*sp;

	defvar	= inquote = quoted = 0;
	bp	= strbuf;
	lp	= --nextp;

	for (EVER)
	{
		if (bp >= &strbuf[BUFSZ - 3])
			error ("$*É preciso aumentar BUFSZ (> %d)", BUFSZ);

		categ = CATEG (catindex = categtb[c = *lp]);

		switch (categ)
		{
			/*
			 *	Separadores e delimitadores.
			 */
		    case USYM:
			if (c == NOCHR)
				goto endloop;

		    case SEPAR:
		    case BSYM:
			if (!inquote)
				goto endloop;

			/* continua no caso abaixo */

			/*
			 *	Letras e dígitos.
			 */
		    case LETTER:
		    case DIGIT:
			*bp++ = c; lp++;
			break;

			/*
			 *	Caracteres inválidos.
			 */
		    case INV:
			error ("Caractere inválido: %X", c);
			lp++;
			break;

			/*
			 *	Caracteres válidos.
			 */
		    case OTHER:
			if (!inquote && !defvar)
			{
				if   (IS_META (c))
					c = CINDEX (catindex);
				elif (c == '=')
					defvar++;
			}

			*bp++ = c; lp++;
			break;

			/*
			 *	É um caractere que acarreta substituições.
			 */
		    case QUOTE:
			switch (CINDEX (catindex))
			{
				/*
				 *	Caractere '\\':
				 */
			    case 0:
				if ((c = *++lp) == NOCHR)
					goto endloop;

				if
				(	inquote && c != '\\' && c != '`' &&
					c != ' ' && c != '$'
				)
					*bp++ = '\\';

				*bp++ = c; lp++;	
				break;

				/*
				 *	Caractere '$'.
				 */
			    case 1:
				tmp   = ++lp;
				get_variable_value (&tmp, 0 /* sem mensagens */);

				*bp++ = VAR_SYMBOL; *bp++ = '0' + inquote;

				for (/* acima */; lp < tmp; lp++)
				{
					if (bp < &strbuf[BUFSZ - 1])
						*bp++ = *lp;
				}

				break;

				/*
				 *	Caractere '\'':
				 */
			    case 2:
				if (inquote)
					{ *bp++ = '\'';	lp++; continue; }

				quoted++;

				while ((c = *++lp) != '\'')
				{
					if (c == NOCHR)
						{ inquote++; goto endloop; }

					if (bp < &strbuf[BUFSZ - 1])
						*bp++ = c;
				}

				lp++;
				break;

				/*
				 *	Caractere '"':
				 */
			    case 3:
				quoted++; inquote ^= 1; lp++;
				break;

				/*
				 *	Caractere '`':
				 */
			    case 4:
				quoted++;

				*bp++ = CMD_SYMBOL;
				*bp++ = '0' + inquote;

				while ((c = *++lp) != '`')
				{
					if (c == NOCHR)
						{ inquote++; *bp++ = CMD_SYMBOL; goto endloop; }

					if (bp < &strbuf[BUFSZ - 2])
						*bp++ = c;
				}

				*bp++ = CMD_SYMBOL; lp++;
				break;

				/*
				 *	Caractere '~':
				 */
			    case 5:
				*bp++ = VAR_SYMBOL; *bp++ = '0' + inquote;
				strcpy (bp, "HOME"); bp += 4;
				lp++;
				break;
			}

			break;

		}	/* end switch (categ) */

	}	/* end for (analisando caracteres da cadeia) */

    endloop:
	nextp       = lp;
	token.t_end = lp - 1;
	*bp++       = NOCHR;		/* Arremata a cadeia obtida */

	if (inquote)
		sherror (LONGMARK, "Cadeia não terminada");

	/*
	 *	Duplica a cadeia.
	 */
	token.t_sval = salloc (bp - strbuf);

	memmove (token.t_sval, strbuf, bp - strbuf);

	if (defvar)
	{
		token.t_progid = P_NOPROG;
		return (DEFVAR);
	}

	/*
	 *	Verifica se é uma palavra reservada.
	 */
	if ((sp = search_sym (token.t_sval)) != NOSYMP && sp->s_type == S_PROG)
	{
		token.t_progid = sp->s_progid;

		return (quoted ? STRING : PROG);
	}

	token.t_progid = P_NOPROG;
	return (STRING);

}	/* end get_string */

/*
 ****************************************************************
 *	Analisa uma cadeia de redirecionamento			*
 ****************************************************************
 */
MAJOR
get_redir (int categ)
{
	RDRC		*rp = &rdrc;
	int		fd  = -1;
	char		*tmp, direction;

	token.t_begin = tmp = nextp - 1;

	if (categ == DIGIT)
	{
		/* O descritor é dado explicitamente */

		fd = strtol ((const char *)tmp, (const char **)&tmp, 0);

		if ((unsigned)fd >= max_fd)
		{
			nextp       = tmp;
			token.t_end = tmp - 1;

			sherror (LONGMARK, "Descritor (%d) inexistente", fd);
			fd = -1;
		}
	}

	direction = *tmp;	/* Guarda o sentido: '>' ou '<' */

	/*
	 *	Guarda o número do descritor (implícito ou explícito).
	 */
	if (fd < 0)
		rp->r_fd = (direction == '>') ? 1 : 0;	/* stdout ou stdin */
	else
		rp->r_fd = fd;

	if (*++tmp == '&')
	{
		if (*++tmp == '-')
		{
			/*
			 *	[n]>&- ou [n]<&-
			 */
			tmp++;
			rp->r_type    = R_DEL;
			rp->r.r_dupfd = -1;
		}
	 	elif (CATEG (categtb [*tmp]) == DIGIT)
		{
			/*
			 *	[n]>&m ou [n]<&m
			 */
			fd = strtol ((const char *)tmp, (const char **)&tmp, 0);

			if ((unsigned)fd >= max_fd)
			{
				nextp	    = tmp;
				token.t_end = tmp - 1;

				sherror
				(	LONGMARK,
					"Descritor de arquivo inexistente: %d",
					fd
				);

				fd         = -1;
				rp->r_type = R_ERROR;
			}

			rp->r_type    = (direction == '>') ? R_TO : R_FROM;
			rp->r.r_dupfd = fd;
		}
		else
		{
			nextp	    = tmp + 1;
			token.t_end = tmp;

			sherror (SHORTMARK, "Esperava '-' ou número");

			rp->r.r_dupfd = -1;
			rp->r_type    = R_ERROR;
		}

	}
	elif (*tmp == direction)
	{
		/* >> ou << */

		tmp++;
		rp->r_type = (direction == '>') ? R_APP : R_SIN;
	}
	else
	{
		rp->r_type = (direction == '>') ? R_OUT : R_IN;
	}

	nextp	    = tmp;
	token.t_end = nextp - 1;

	return (REDIREC);

}	/* end get_redir */

/*
 ****************************************************************
 *	Analisa uma variável da forma "$..."			*
 ****************************************************************
 *
 *	"pp" entra apontando para o caractere após o '$'
 *	"pp" sai   apontando para o fim do nome da variável
 */
char *
get_variable_value (char **pp, int verbose)
{
	char		*lp, *cp, *beginp, c;
	uchar		categ;
	SYM		*sp;
	static char	str[8];

	lp = *pp;
	token.t_begin = lp - 1;

	(*pp)++;

	/*
	 *	Trata as variáveis especiais.
	 */
	switch (*lp)
	{
		/*
		 *	$: = número do próximo comando.
		 */
	    case ':':
		snprintf (str, sizeof (str), "%d", last_cmd + 1);
		return (str);

		/*
		 *	$? = código de término do último comando.
		 */
	    case '?':
		snprintf (str, sizeof (str), "%d", last_cmd_code);
		return (str);

		/*
		 *	$# = número de parâmetros posicionais.
		 */
	    case '#':
		snprintf (str, sizeof (str), "%d", npospar - 1);
		return (str);

		/*
		 *	$$ = PID da Shell.
		 */
	    case '$':
		snprintf (str, sizeof (str), "%d", my_pid);
		return (str);

		/*
		 *	$! = PID do comando em background.
		 */
	    case '!':
		snprintf (str, sizeof (str), "%d", last_bg_pid);
		return (str);

		/*
		 *	$- = cadeia com as opções da Shell.
		 */
	    case '-':
		cp = str;

		if (dflag)	*cp++ = 'd';
		if (quick_exit)	*cp++ = 'e';
		if (!rewriting)	*cp++ = 'r';
		if (vflag)	*cp++ = 'v';

		*cp = NOCHR;

		return (str);

	    case '@':
	    case '*':
		return (all_params ());

		/*
		 *	$~ = nome do diretório corrente
		 */
	    case '~':
		return (current_wd);

		/*
		 *	$% = nome da máquina.
		 */
	    case '%':
		return ((char *)scb.y_uts.uts_nodename);

		/*
		 *	$& = prompt de acordo com os privilégios.
		 */
	    case '&':
		return (user_id == 0 ? "#" : "$");

	    default:
		*pp = lp;
		break;
	}

	if (CATEG (categtb [*lp]) == DIGIT)
	{
		/* É um parâmetro posicional: obtém seu número */

		(*pp)++;

		if ((c = *lp - '0') < npospar)
			return ((char *)pospar[c]);

		if (verbose)
			error ("Parâmetro posicional \"$%d\" indefinido", c);

		return (NOSTR);
	}

	/*
	 *	O nome da variável pode vir delimitado por {}.
	 */
	if (*lp == '{')
		lp++;

	/*
	 *	Acha o fim do identificador, sem fazer cópias.
	 */
	beginp = lp;

	while ((categ = CATEG (categtb[c = *lp])) == LETTER || categ == DIGIT)
		lp++;

	token.t_end = lp - 1;

	if (beginp[-1] == '{')
	{
		/* Havia '{' no início: é preciso fechar com '}' */

		if (c != '}')
		{
			sherror (LONGMARK, "'}' esperado");
			return (INVSTR);
		}

		*pp = lp + 1;
		token.t_end++;
	}
	else
	{
		*pp = lp;
	}

	if (beginp == lp)
	{
		if (verbose)
			error ("Variável mal formada");

		return (INVSTR);
	}

	/*
	 *	Salva o caractere seguinte ao nome da variável e coloca
	 *	um NOCHR em seu lugar.
	 */
	c   = *lp; *lp = NOCHR;

	/*
	 *	Trata-se de uma variável: realiza a busca na tabela
	 *	de símbolos para obter o seu valor.
	 */
	sp = search_sym (beginp);

	if (sp == NOSYMP || sp->s_type != S_VAR)
	{
		if (verbose)
			error ("Variável \"%s\" não definida", beginp);

		cp = INVSTR;
	}
	else
	{
		cp = (char *)sp->s_value;
	}

	*lp = c;	/* restaura o caractere seguinte */

	return (cp);

}	/* end get_variable_value */

/*
 ****************************************************************
 *	Escreve o "prompt" dado					*
 ****************************************************************
 */
void
put_prompt (const char *p)
{
	char		*prompt[2], **argv, **ap;
	char		*save_linebuf = linebuf;

	linebuf		= (char *)p;
	nextp		= linebuf + 1;
	token.t_begin	= linebuf - 1;

	token.t_major	= get_string ();

	linebuf		= save_linebuf;

	prompt[0] = token.t_sval;
	prompt[1] = NOSTR;

	if ((argv = expand_args (prompt, NULL)) != NOSSTR)
	{
		for (ap = argv; *ap != NOSTR; ap++)
			fprintf (stderr, "%s", *ap);

		fflush (stderr);

		free_args (argv);
	}
	else
	{
		fprintf (stderr, "%c ", user_id == 0 ? '#' : '$');
	}


}	/* end put_prompt */

/*
 ****************************************************************
 *	Copia o texto da entrada padrão				*
 ****************************************************************
 */
int
copyupto (const char *word, const char *tmpname)
{
	int	nread, size, fd, ret;
	char	ch;

	if ((fd = creat (tmpname, 0600)) < 0)
	{
		error ("*Não consegui criar o arquivo \"%s\"", tmpname);
		return (-1);
	}

	size = strlen (word);

	for (EVER)
	{
		if ((nread = read_line (input_file, 0)) < 0)
			{ ret = -1; break; }

		if (nread >= size)
		{
			ch = linebuf[size];
			linebuf[size] = NOCHR;

			if (streq (linebuf, word))
				{ ret = 0; break; }

			linebuf[size] = ch;
		}

		linebuf[nread] = '\n';
		linebuf[nread + 1] = NOCHR;

		if (write (fd, linebuf, nread + 1) != nread + 1)
		{
			error ("*Não consegui escrever em \"%s\"", tmpname);
			ret = -1; break;
		}

	}	/* end for (EVER) */

	close (fd);
	return (ret);

}	/* end copyupto */

/*
 ****************************************************************
 *	Gera uma cadeia com os parâmetros posicionais		*
 ****************************************************************
 */
char *
all_params (void)
{
	int		len, ind;
	char		*str, *cp;

	if (npospar <= 0)
		return ("");

	for (len = 0, ind = 1; ind < npospar; ind++)
		len += strlen (pospar[ind]) + 1;

	if (len == 0 || (cp = str = salloc (len)) == NOSTR)
		return (NOSTR);

	for (ind = 1; ind < npospar; ind++)
	{
		strcpy (cp, pospar[ind]);
		cp = strend (cp);
		*cp++ = ' ';
	}

	cp[-1] = NOCHR;

	return (str);

}	/* end all_params */

/*
 ****************************************************************
 *	Processa os metacaracteres				*
 ****************************************************************
 */
int
hasmeta (char *str)
{
	int		meta;
	char		c;
	static char	meta_char[] = " *?[]^!";

	for (meta = 0; (c = *str) != NOCHR; str++)
	{
		if (c >= 1 && c <= 6)
		{
			*str = meta_char[c];
			meta++;
		}
	}

	return (meta);

}	/* end hasmeta */

/*
 ****************************************************************
 *	Troca os metacaracteres por seus equivalentes		*
 ****************************************************************
 */
void
putmeta (char *str)
{
	char		c;

	for (/* vazio */; (c = *str) != NOCHR; str++)
	{
		if (IS_META (c))
			*str = CINDEX (categtb[c]);
	}

}	/* end putmeta */
