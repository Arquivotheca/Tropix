/*
 ****************************************************************
 *								*
 *			fnrgets.c				*
 *								*
 *	Leitura de linhas					*
 *								*
 *	Versão	4.4.0, de 11.11.02				*
 *		4.5.0, de 12.11.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */
#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <signal.h>
#include <setjmp.h>

#include "../h/categ.h"

/*
 ******	Definições locais ***************************************
 */
#define	EVER		;;

#define	ESC		0x1B		/* Caractere Escape */
#define	CTL_A		0x01		/* Caractere <^A> */
#define	CTL_G		0x07		/* Caractere <^G> */
#define	CTL_Q		0x11		/* Caractere <^Q> */
#define	CTL_S		0x13		/* Caractere <^S> */
#define	CTL_Z		0x1A		/* Caractere <^Z> */
#define	DEL		0x7F		/* Caractere <Del> */

#define	PUTS(s,l)	if (echo) write (fileno (stdout), (s), (l))

#define	INCR		512		/* Incremento do tamanho da linha */

#define	SAVE_CHAR(c)					\
{							\
	if (lp >= endlp)				\
	{						\
		char	*newline;			\
							\
		newline = realloc (line, linesz + INCR);\
							\
		if (newline == NOSTR)			\
			break;				\
							\
		lp	= newline + (lp - line);	\
		line	= newline;			\
		linesz += INCR;				\
		endlp	= line + (linesz - 1);		\
	}						\
							\
	count++;					\
	*lp++ = (c);					\
}

/*
 ******	Tabela de acentuação ************************************
 */
typedef struct
{
	ushort	a_src;	/* Fonte: pares */
	ushort	a_dst;	/* Destino: resultado da conversão */

}	ACENTO;

#define	OR(a, b)	(((a) << 8) | (b))

static ACENTO	acentos[] =
{
	OR ('~', 'A'),	0xC3,	/* Ã */
	OR ('~', 'a'),	0xE3,	/* ã */
	OR ('~', 'O'),	0xD5,	/* Õ */
	OR ('~', 'o'),	0xF5,	/* õ */
	OR (',', 'C'),	0xC7,	/* Ç */
	OR (',', 'c'),	0xE7,	/* ç */
	OR (',', 'z'),	0xDF,	/* ß */
	OR ('\'','A'),	0xC1,	/* Á */
	OR ('\'','a'),	0xE1,	/* á */
	OR ('\'','E'),	0xC9,	/* É */
	OR ('\'','e'),	0xE9,	/* é */
	OR ('\'','I'),	0xCD,	/* Í */
	OR ('\'','i'),	0xED,	/* í */
	OR ('\'','O'),	0xD3,	/* Ó */
	OR ('\'','o'),	0xF3,	/* ó */
	OR ('\'','U'),	0xDA,	/* Ú */
	OR ('\'','u'),	0xFA,	/* ú */
	OR ('^', 'A'),	0xC2,	/* Â */
	OR ('^', 'a'),	0xE2,	/* â */
	OR ('^', 'E'),	0xCA,	/* Ê */
	OR ('^', 'e'),	0xEA,	/* ê */
	OR ('^', 'I'),	0xCE,	/* Î */
	OR ('^', 'i'),	0xEE,	/* î */
	OR ('^', 'O'),	0xD4,	/* Ô */
	OR ('^', 'o'),	0xF4,	/* ô */
	OR ('^', 'U'),	0xDB,	/* Û */
	OR ('^', 'u'),	0xFB,	/* û */
	OR ('`', 'A'),	0xC0,	/* À */
	OR ('`', 'a'),	0xE0,	/* à */
	OR ('`', 'E'),	0xC8,	/* È */
	OR ('`', 'e'),	0xE8,	/* è */
	OR ('`', 'I'),	0xCC,	/* Ì */
	OR ('`', 'i'),	0xEC,	/* ì */
	OR ('`', 'O'),	0xD2,	/* Ò */
	OR ('`', 'o'),	0xF2,	/* ò */
	OR ('`', 'U'),	0xD9,	/* Ù */
	OR ('`', 'u'),	0xF9,	/* ù */
	OR (':', 'A'),	0xC4,	/* Ä */
	OR (':', 'a'),	0xE4,	/* ä */
	OR (':', 'E'),	0xCB,	/* Ë */
	OR (':', 'e'),	0xEB,	/* ë */
	OR (':', 'I'),	0xCF,	/* Ï */
	OR (':', 'i'),	0xEF,	/* ï */
	OR (':', 'O'),	0xD6,	/* Ö */
	OR (':', 'o'),	0xF6,	/* ö */
	OR (':', 'U'),	0xDC,	/* Ü */
	OR (':', 'u'),	0xFC,	/* ü */
	OR ('¨', 'A'),	0xC4,	/* Ä */
	OR ('¨', 'a'),	0xE4,	/* ä */
	OR ('¨', 'E'),	0xCB,	/* Ë */
	OR ('¨', 'e'),	0xEB,	/* ë */
	OR ('¨', 'I'),	0xCF,	/* Ï */
	OR ('¨', 'i'),	0xEF,	/* ï */
	OR ('¨', 'O'),	0xD6,	/* Ö */
	OR ('¨', 'o'),	0xF6,	/* ö */
	OR ('¨', 'U'),	0xDC,	/* Ü */
	OR ('¨', 'u'),	0xFC,	/* ü */

	0,		0
};

static ACENTO	*end_acentos = &acentos[sizeof (acentos) / sizeof (ACENTO) - 1];

static int	winsize_changed;

/*
 ****************************************************************
 *	Lê uma linha						*
 ****************************************************************
 */
int
fnrgets (char **given_line, int *given_size, char **given_lp, FILE *fp)
{
	TERMIO		termio;
	int		c, count, linesz, termio_tmin;
	char		*line, *lp, *endlp;
	SIG_ENUM	sig = SIGNULL;

	/*
	 *	Inicializa as variáveis a partir dos parâmetros dados
	 */
	if ((line = *given_line) == NOSTR)
	{
		linesz	= 0;
		lp	= endlp = NOSTR;
	}
	else
	{
		linesz	= *given_size;
		lp	= given_lp ? *given_lp : line;
		endlp	= line + (linesz - 1);
	}

	/*
	 *	Verifica se estamos lendo de um terminal
	 */
	if (ioctl (fileno (fp), TCGETS, &termio) < 0)
	{
		/* NÃO é um terminal: lê normalmente */

		for (count = 0; (c = getc (fp)) != '\n' && c != EOF; /* vazio */)
			SAVE_CHAR (c);
	}
	else
	{
		int	acento;
		int	echo = termio.c_lflag & ECHO;

		/* Coloca apenas a entrada do terminal em modo RAW */

		winsize_changed = 0;

		termio_tmin	= termio.t_min;
		termio.c_lflag &= ~(ECHO|ICANON|ISIG);
		termio.t_min	= 1;

		if (ioctl (fileno (fp), TCSETNR, &termio) < 0)
			return (-1);

		for (count = acento = 0; /* abaixo */; /* abaixo */)
		{
			c = getc (fp);		/* Lê um caractere */

			if (c < 0)
				continue;

			/* Trata os caracteres de controle */

			if (c == '\r' || c == '\n')
				{ c = '\n'; break; }

			if (c == termio.c_cc[VEOF])
				{ PUTS ("<EOF>", 5); c = EOF; break; }

			if (c == termio.c_cc[VINTR])
				{ PUTS ("<INTR>", 6); sig = SIGINT; break; }

			if (c == termio.c_cc[VQUIT])
				{ PUTS ("<QUIT>", 6); sig = SIGQUIT; break; }

			if (c == termio.c_cc[VKILL])
			{
				if (count == 0)
				{
					PUTS ("\007", 1);
				}
				else
				{
					do
					{
						PUTS ("\e[D \e[D", 7);

						if (*--lp <= CTL_Z)
							PUTS ("\e[D \e[D", 7);
					}
					while (--count > 0);
				}

			   /*** count  = 0; ***/
				acento = 0; continue;

			}	/* end if (KILL) */

			if (c == termio.c_cc[VERASE] || c == termio.c_cc[VAERASE])
			{
				if (count == 0)
				{
					PUTS ("\007", 1);
				}
				else
				{
					count--; PUTS ("\e[D \e[D", 7);

					if (*--lp <= CTL_Z)
						PUTS ("\e[D \e[D", 7);
				}

				acento = 0; continue;

			}	/* end if (ERASE ou AERASE) */

			if (c == termio.c_cc[VRETYPE])
			{
				if (count > 0)
				{
					PUTS (&c, 1); PUTS ("\r\n", 2);
					PUTS (lp - count, count);
				}

			   /***	"acento" é mantido ***/
				continue;
			}

			/* A seta ascendente gera o comando "gh" */

			if (c == ESC)
			{
				if (getc (fp) == '[' && getc (fp) == 'A')
					{ SAVE_CHAR ('g'); SAVE_CHAR ('h'); c = '\n'; break; }

				acento = 0; continue;
			}

			/* Trata <^Q> e <^S> */

			if (c == CTL_Q)
				{ ioctl (fileno (fp), TCXONC, 1); continue; }

			if (c == CTL_S)
				{ ioctl (fileno (fp), TCXONC, 0); continue; }

			/* Trata os acentos */

			if (IS_ACCENT (c))
				{ SAVE_CHAR (c); PUTS (&c, 1); acento = c; continue; }

			if (c == '\\' && acento)
				{ acento = 0; continue; }

			/* Trata os demais caracteres */

			if (acento == 0)
			{
				/* NÃO vem precedido de acento */

				SAVE_CHAR (c);
			}
			else
			{
				/* Processa os acentos */

				const ACENTO	*ap;

				acento = OR (acento, c);

				/* Procura na tabela */

				end_acentos->a_src = acento;	/* sentinela */

				for (ap = acentos; ap->a_src != acento; ap++)
					/* vazio */;

				if (ap < end_acentos)
					{ c = lp[-1] = ap->a_dst; PUTS ("\e[D", 3); }
				else
					SAVE_CHAR (c);	/* NÃO achou */

				acento = 0;

			}	/* end if (precedido de acento) */

			/* Ecoa o caractere */

			PUTS (&c, 1);

			if (c == CTL_G)			/* Processa a campainha */
				PUTS ("^G", 2);

		}	/* end for (lendo caracteres) */

		PUTS ("\r\n", 2);

		/* Restaura o modo anterior */

		if (winsize_changed)
		{
			TERMIO		*tp = alloca (sizeof (TERMIO));

			if (ioctl (fileno (fp), TCGETS, tp) >= 0)
			{
				termio.t_nline = tp->t_nline;
				termio.t_ncol  = tp->t_ncol;
				termio.t_page  = tp->t_page;
			}
		}

		termio.c_lflag |= ECHO|ICANON|ISIG;
		termio.t_min	= termio_tmin;

		ioctl (fileno (fp), TCSETNR, &termio);

	}	/* end if (é terminal) */

	/*
	 *	Providências finais
	 */
	*lp	    = '\0';		/* Arremata o fim da linha */
	*given_size = linesz;		/* O novo tamanho */
	*given_line = line;		/* A nova linha */

	if (given_lp)
		*given_lp = lp;		/* A nova continuação */

	if (count == 0 && c == EOF)	/* Processa o EOF */
		return (EOF);

	if (sig != SIGNULL)		/* Processa os sinais */
	{
		ioctl (fileno (fp), TCFLSH, 1);
		raise (sig);
	}

	return (c);			/* Retorna o último char lido */

}	/* end fnrgets */

/*
 ****************************************************************
 *	Trata o SIGWINCH					*
 ****************************************************************
 */
void
handle_sigwinch (int signo, ...)
{
	signal (SIGWINCH, handle_sigwinch);

	winsize_changed++;

}	/* end handle_sigwinch */

#ifdef	DEBUG
jmp_buf	jmpbuf;

void
sigint (int sig, ...)
{
	signal (sig, sigint);

	longjmp (jmpbuf, 1);
}

int
main (void)
{
	int	n;
	char	*buf, *bufp;

	signal (SIGINT,  sigint);
	signal (SIGQUIT, sigint);

	if (setjmp (jmpbuf) == 0)
		buf = NOSTR;
	else
		bufp = buf;

	while (fnrgets (&buf, &n, &bufp, stdin) >= 0)
	{
		printf ("Lido: <%s>\n", buf);

		if (bufp < buf + n - 2)
			{ *bufp++ = ';';	*bufp++ = ' '; }
	}

	return (0);
}
#endif	DEBUG
