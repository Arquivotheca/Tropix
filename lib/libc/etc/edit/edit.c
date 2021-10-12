/*
 ****************************************************************
 *								*
 *			edit.c					*
 *								*
 *	Edição de uma Linha para programas interativos		*
 *								*
 *	Versão	1.0.0, de 12.03.87				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo:	edit						*
 *		libc/etc					*
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
#include <string.h>
#include <termio.h>
#include <ctype.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	LINESZ	256		/* Tamanho max. para as Linhas */

#define	NOSTR	(char *)NULL
#define	elif	else if

typedef enum { 	false, 	true  }	bool;

/*
 ****************************************************************
 *	Definição de constantes e macros locais			*
 ****************************************************************
 */
#define		BS	010
#define		CTLR	022
#define		CTLU	025
#define		ESC	033
#define		DEL	0177
#define		DELCH	'?'

#define		all(c)		(c == 'e' || c == '0' || c == 'b')

#define		isaccent(c)	(c == ',' || c == ':' || c == '\'' ||	\
					c == '`' || c == '^' || c == '~')

#define		delete(a, b)	strmove (Line + a, Line + b + 1)
#define		insch(c, o)	strmove (Line + o + 1, Line + o), Line[o] = c

/*
 ****************************************************************
 *	Definição de tipos locais				*
 ****************************************************************
 */
/*
 *	Classificação dos comandos quanto à sua Funcionalidade:
 */
typedef	enum
{
	F_NULL,		/* Invalido */
	F_MOVE,		/* Movimento do Cursor */
	F_DELETE,	/* Remoção de Caracteres */
	F_CHANGE,	/* Remoção e Inserção de Caracteres */
	F_INSERT,	/* Inserção de Caracteres */
	F_REPLACE,	/* Substituição de um caractere */
	F_RETYPE,	/* Reescreve a linha em edição */
	F_ORIGINAL,	/* Retorna a linha ao seu estado Original */
	F_UNDO,		/* Retorna a linha ao seu estado anterior */
	F_END		/* Fim da edição */

}	FUN;

/*
 *	Estrutura Comando:
 */
typedef	struct
{
	FUN	c_fun;		/* Função */
	char	c_obj;		/* Objeto */

}	COM;

/*
 ****************************************************************
 *	Variáveis locais					*
 ****************************************************************
 */
static char		*Line;		/* Linha em edição */
static const char	*Prompt;

static int		Order;		/* Ordem do caracter corrente */
static char		*Original;	/* Estado original da linha */

/*
 ****************************************************************
 *	Estruturas de dados para a atualização do vídeo		*
 ****************************************************************
 */
#define		MAX	(2*LINESZ -1)

static char	*New = NOSTR;		/* Novo, a ser atualizado */
static char	*Old = NOSTR;		/* Estado anterior */

static int	Column;		/* Coluna do caracter corrente */
static int	First;		/* Primeira coluna ocupada pela linha */

/*
 ****************************************************************
 *	Tabela de acentos					*
 ****************************************************************
 */
typedef	struct
{
	ushort	a_fonte;	/* Fonte: pares */
	ushort	a_destino;	/* Destino: resultado da conversão */

}	ACENTO;

#define	OR(a, b)	((a << 8) | b)

static const ACENTO	acento[] =
{
	OR ('~', 'A'),	0xC3,
	OR ('~', 'a'),	0xE3,
	OR ('~', 'O'),	0xD5,
	OR ('~', 'o'),	0xF5,
	OR (',', 'C'),	0xC7,
	OR (',', 'c'),	0xE7,
	OR ('\'','A'),	0xC1,
	OR ('\'','a'),	0xE1,
	OR ('\'','E'),	0xC9,
	OR ('\'','e'),	0xE9,
	OR ('\'','I'),	0xCD,
	OR ('\'','i'),	0xED,
	OR ('\'','O'),	0xD3,
	OR ('\'','o'),	0xF3,
	OR ('\'','U'),	0xDA,
	OR ('\'','u'),	0xFA,
	OR ('^', 'A'),	0xC2,
	OR ('^', 'a'),	0xE2,
	OR ('^', 'E'),	0xCA,
	OR ('^', 'e'),	0xEA,
	OR ('^', 'I'),	0xCE,
	OR ('^', 'i'),	0xEE,
	OR ('^', 'O'),	0xD4,
	OR ('^', 'o'),	0xF4,
	OR ('^', 'U'),	0xDB,
	OR ('^', 'u'),	0xFB,
	OR ('`', 'A'),	0xC0,
	OR ('`', 'a'),	0xE0,
	OR ('`', 'E'),	0xC8,
	OR ('`', 'e'),	0xE8,
	OR ('`', 'I'),	0xCC,
	OR ('`', 'i'),	0xEC,
	OR ('`', 'O'),	0xD2,
	OR ('`', 'o'),	0xF2,
	OR ('`', 'U'),	0xD9,
	OR ('`', 'u'),	0xF9,
	OR (':', 'A'),	0xC4,
	OR (':', 'a'),	0xE4,
	OR (':', 'E'),	0xCB,
	OR (':', 'e'),	0xEB,
	OR (':', 'I'),	0xCF,
	OR (':', 'i'),	0xEF,
	OR (':', 'O'),	0xD6,
	OR (':', 'o'),	0xF6,
	OR (':', 'U'),	0xDC,
	OR (':', 'u'),	0xFC,
	0,		0
};

/*
 ******	Protótipos de funções ***********************************
 */
static FUN	read_command (COM *);
static int	insert (int, const TERMIO *);
static int	object (char);
static void	cmove (int);
static void	move (int);
static int	column (int);
static void	beep (void);
static void	display (int);
static void	twrite (char *);
static void	display (int);
static void	twrite (char *);
static void	redisplay (int);
static bool	space (const char *);

/*
 ****************************************************************
 *	Edição de uma Linha					*
 ****************************************************************
 */
int
edit (const char *prompt, const char *original, char *line)
{
	COM		edition;
	FUN		f;
	int		new_order;
	char		ch;
	TERMIO		termio, oldtermio;

	/*
	 *	Verifica se estamos em um Terminal
	 */
	if (!isatty (fileno (stdin)) || !isatty (fileno (stdout)))
	{
		write (2, "edit: Só edito em terminais\n", 28);
		return (-1);
	}

	/*
	 *	Poe o terminal em RAW
	 */
	ioctl (1, TCGETS, &oldtermio);	termio = oldtermio;

	termio.c_lflag &= ~(ECHO|ICANON|ISIG);
	termio.c_oflag &= ~OPOST;
	termio.t_min = 1;

	ioctl (1, TCSETNR, &termio);

	/*
	 *	Inicializa Variáveis Globais
	 */
	Prompt = prompt;
	Line = line;
	strcpy (Line, original);

	if ((Original = malloc (strlen (original) + 1)) != NOSTR)
		strcpy (Original, original);

	/*
	 *	Aloca memoria para Atualização do Video
	 */
	New = malloc (MAX + 1);
	Old = malloc (MAX + 1);

	if (Old == NOSTR)
	{
		write (2, "edit: não consigo memória dinâmica\n", 36);
		return (-1);
	}

	/*
	 *	Escreve a linha
	 *	e posiciona o cursor sobre seu primeiro caractere
	 */
	display (0);

	/*
	 *	Interpreta e Executa comandos de Edição
	 */
	while ((f = read_command (&edition)) != F_END) switch (f)
	{
	    case F_NULL:
		beep ();
		continue;

	    case F_MOVE:
		if ((new_order = object (edition.c_obj)) >= 0)
			move (new_order);
		else
			beep ();
		continue;

	    case F_DELETE:
		if ((new_order = object (edition.c_obj)) >= 0)
		{
			if (new_order < Order)
			{
				/*
				 *	Remoção à esquerda
				 */
				if (all (edition.c_obj))
					delete (new_order, Order);
				else
					delete (new_order, Order - 1);
				redisplay (new_order);
			}
			else
			{
				/*
				 *	Remoção à direita
				 */
				if (all (edition.c_obj))
					delete (Order, Order);
				else
					delete (Order, new_order - 1);
				redisplay (Order);
			}
		}
		else
		{
			beep ();
		}
		continue;

	    case F_CHANGE:
		if ((new_order = object (edition.c_obj)) >= 0)
		{
			if (new_order > Order)
			{
				/*
				 *	Substituição à direita
				 */
				if (all (edition.c_obj))
					delete (Order, new_order);
				else
					delete (Order, new_order - 1);

				new_order = Order;
			}
			elif (new_order < Order)
			{
				/*
				 *	Substituição à direita
				 */
				if (all (edition.c_obj))
					delete (new_order, Order);
				else
					delete (new_order, Order - 1);
			}

			if (insert (new_order, &termio) != ESC)
				goto end_edit;
		}
		else
		{
			beep ();
		}
		continue;

	    case F_INSERT:
		if ((new_order = object (edition.c_obj)) >= 0)
		{
			if (insert (new_order, &termio) != ESC)
				goto end_edit;
		}
		else
		{
			beep ();
		}
		continue;

	    case F_REPLACE:
		while ((ch = getchar ()) < ' ' || ch >= DEL)
			beep ();

		Line[Order] = ch;
		redisplay (Order);
		continue;

	    case F_RETYPE:
		printf ("\r\n");
		display (Order);
		continue;

	    case F_ORIGINAL:
		if (Original != NOSTR)
		{
			strcpy (Line, Original);
			redisplay (0);
		}
		else
		{
			beep ();
		}
		continue;

	    case F_UNDO:
		beep ();
		continue;

	    case F_END:
		goto end_edit;

	}	/* while (read_command) */

    end_edit:
	printf ("\r\n");

	ioctl (1, TCSETNR, &oldtermio);

	if (Original != NOSTR)
		free (Original);

	free (New); free (Old);

	return (0);

}	/* end edit */

/*
 ****************************************************************
 *	Leitura do Comando (preenchimento da struct (FUN)	*
 ****************************************************************
 */
static FUN	
read_command (register COM *c)
{
	register char	ch;

	switch (ch = getchar ())
	{
	    case '0':
	    case '$':
	    case ' ':
	    case 'l':
	    case '\b':
	    case DEL:
	    case 'h':
	    case 'w':
	    case 'e':
	    case 'b':
		c->c_fun = F_MOVE;
		c->c_obj = ch;
		break;

	    case ESC:
		c->c_fun = F_NULL;

		if (getchar () == '[') switch (ch = getchar ())
		{
		    case 'C':
			c->c_fun = F_MOVE;
			c->c_obj = 'l';
			break;

		    case 'D':
			c->c_fun = F_MOVE;
			c->c_obj = 'h';
			break;
		}

		break;

	    case 'd':
		c->c_fun = F_DELETE;
		c->c_obj = getchar ();
		break;

	    case 'c':
		c->c_fun = F_CHANGE;
		c->c_obj = getchar ();
		break;

	    case 'i':
		c->c_fun = F_INSERT;
		c->c_obj = '\0';
		break;

	    case 'a':
		c->c_fun = F_INSERT;
		c->c_obj = ' ';
		break;

	    case CTLR:
	    case '\f':
		c->c_fun = F_RETYPE;
		break;

	    case 'u':
		c->c_fun = F_UNDO;
		break;

	    case 'U':
		c->c_fun = F_ORIGINAL;
		break;

	    case 'q':
	    case '\r':
	    case '\n':
		c->c_fun = F_END;
		break;

	    case 'r':
		c->c_fun =F_REPLACE;
		break;

	    case 'A':
		c->c_fun = F_INSERT;
		c->c_obj = '$';
		break;

	    case 'C':
		c->c_fun = F_CHANGE;
		c->c_obj = '$';
		break;

	    case 'D':
		c->c_fun = F_DELETE;
		c->c_obj = '$';
		break;

	    case 'I':
		c->c_fun = F_INSERT;
		c->c_obj = '0';
		break;

	    case 'X':
		c->c_fun = F_DELETE;
		c->c_obj = '\b';
		break;

	    case 'x':
		c->c_fun = F_DELETE;
		c->c_obj = ' ';
		break;

	    case 's':
		c->c_fun = F_CHANGE;
		c->c_obj = ' ';
		break;

	    default:
		c->c_fun = F_NULL;

	}	/* switch (getchar) */

	return (c->c_fun);

}	/* end read_command */

/*
 ****************************************************************
 *	Inserção em line					*
 ****************************************************************
 */
static int
insert (int start, const TERMIO *tp)	/* "start": Ordem para o Ponto inicial de inserção */
{
	register char		ch;

	register bool		isokey;		/* Teclado ISO */
	register int		oldchar,	/* Caractere de Acentuação Pendente */
				och;

	register int		orc;
	register const ACENTO	*ap;

	isokey = (tp->c_lflag & ISOKEY) != 0;
	oldchar = 0;

	/*
	 *	Insere Espaco para o Cursor
	 */
	insch (' ', start);

	/*
	 *	Atualiza Video com o Cursor no Ponto de Inserção
	 */
	redisplay (start);

	/*
	 *	Insere caracteres teclados até ESC
	 */
	while ((ch = getchar ()) != ESC  &&  ch != '\n'  &&  ch != '\r')
	{
		/*
		 *	Remoção do Caractere Anterior
		 */
		if (ch == tp->t_erase || ch == tp->t_aerase)
		{
			if (Order > start)
			{
				delete (Order - 1, Order - 1);
				redisplay (Order - 1);
			}
			else
			{
				beep ();
			}
			goto getnextchar;
		}

		/*
		 *	Remoção do texto inserido até este ponto
		 */
		if (ch == CTLU)
		{
			if (Order > start)
			{
				delete (start, Order - 1);
				redisplay (start);
			}
			else
			{
				beep ();
			}
			goto getnextchar;
		}

		/*
		 *	Verificação do Tamanho da Linha
		 */
		if (strlen (Line) >= LINESZ)
		{
			beep ();
			goto getnextchar;
		}

		/*
		 *	Verifica a Existencia de um Acento Pendente
		 */
		if (oldchar)
		{
			och = oldchar;
			oldchar = 0;

			/*
			 *	Tratamento dos ACENTOS
			 */
			if   (ch == '\\')
				goto getnextchar;

			orc = OR (och, ch); 

			for (ap = acento; ap->a_fonte; ap++)
			{
				if (ap->a_fonte == orc)
				{
					/*
					 *	Achou o acento dado
					 */
					Line[Order - 1] = ap->a_destino;
					redisplay (Order);
					goto getnextchar;
				} 
			}

			/*
			 *	O acento dado não existe
			 */
			insch (ch, Order);
			redisplay (Order + 1);

			/*
			 *	Se acento, fica pendente
			 */
			if (isaccent (ch))
				oldchar = ch;

			goto getnextchar;

		}	/* if (oldchar) */

		/*
		 *	Se acento, insere e fica pendente
		 */
		if (isaccent (ch) && ! isokey)
		{
			oldchar = ch;
			insch (ch, Order);
			redisplay (Order + 1);
			goto getnextchar;
		}

		/*
		 *	Caractere comum: insere
		 */
		insch (ch, Order);
		redisplay (Order + 1);

		/*
		 *	Retorna ao inicio do Loop para a
		 *	leitura do proximo caractere
		 */
	    getnextchar:
		/* vazio */;

	}	/* while getchar != ESC */

	/*
	 *	Remove Espaco para o Cursor
	 */
	delete (Order, Order);
	redisplay (Order);

	return (ch);

}	/* end insert */

/*
 ****************************************************************
 *	Calcula ordem correspondente a um Objeto.		*
 ****************************************************************
 */
static int	
object (char obj)
{
	register int	order;

	order = Order;

	switch (obj)
	{
	    case '\0':
		return (Order);

	    /*
	     *	Avanca um caractere
	     */
	    case ' ':
	    case 'l':
		if (Order < strlen (Line))
			return (Order + 1);
		else
			return (-1);

	    /*
	     *	Retrocede um caractere
	     */
	    case '\b':
	    case 'h':
	    case DEL:
		if (Order > 0)
			return (Order - 1);
		else
			return (-1);

	    /*
	     *	Primeiro Caractere
	     */
	    case '0':
		return (0);

	    /*
	     *	Ultimo Caractere
	     */
	    case '$':
		return (strlen (Line));

	    /*
	     *	Avanca para o inicio da Proxima Palavra
	     */
	    case 'w':
		if (ispunct (Line[order]) || iscntrl (Line[order]))
		{
			do
				order++;
			while (Line[order] != '\0' && ispunct (Line[order]));
		}
		elif (isalnum (Line[order]))
		{
			do
				order++;
			while (Line[order] != '\0' && isalnum (Line[order]));
		}

		while (Line[order] != '\0' && isspace (Line[order]))
			order++;
		return (order);

	    /*
	     *	Avanca para o proximo Fim de Palavra
	     */
	    case 'e':
		order++;
		while (Line[order] != '\0' && isspace (Line[order]))
			order++;

		if (ispunct (Line[order]))
		{
			while (Line[++order] != '\0' && ispunct (Line[order]))
				/* vazio */;
		}
		elif (isalnum (Line[order]))
		{
			while (Line[++order] != '\0' && isalnum (Line[order]))
				/* vazio */;
		}

		if (order > 0)
			order--;
		return (order);

	    /*
	     *	Retrocede para o Inicio de Palavra Anterior
	     */
	    case 'b':
		if (order == 0)
			return (0);
		order--;
		while (order > 0 && isspace (Line[order]))
			order--;

		if (ispunct (Line[order]))
		{
			while (--order > 0 && ispunct (Line[order]))
				/* vazio */;
		}
		elif (isalnum (Line[order]))
		{
			while (--order > 0 && isalnum (Line[order]))
				/* vazio */;
		}

		if (Line[order] != '\0' && order != 0)
			order++;
		return (order);
	}

	return (-1);

}	/* end object */

/*
 ****************************************************************
 *	Altera a posição corrente				*
 ****************************************************************
 */
static void	
move (register int new_order)
{
	register int	max;

#ifdef	DEBUG
	fprintf (stderr, "move: O:%d, o:%d\n", Order, new_order);
#endif	DEBUG

	max = strlen (Line) - 1;
	if (new_order > max)
	{
		if (Line[Order+1] == '\0')
			beep ();
		new_order = max;
	}
	cmove (column (new_order));
	Order = new_order;

}	/* end move */

/*
 ****************************************************************
 *	Move Cursor para coluna.				*
 ****************************************************************
 */
static void
cmove (register int col)
{
#ifdef	NON_DESTR_TABS
	register int	nextstop;
#endif	NON_DESTR_TABS

#ifdef	DEBUG
	fprintf (stderr, " cmove: C:%d, c:%d\n", Column, col);
#endif	DEBUG

	/*
	 *	Move para coluna à esquerda da atual
	 */
	if (Column > col)
	{
		if (Column - col > col + 1  &&  Column < 80)
		{
			putchar ('\r');
			Column = 0;
		}
		else
		{
			while (Column > col)
			{
				putchar ('\b');
				Column--;
			}
		}
	}

	/*
	 *	Move para coluna à direita da atual
	 */
#ifdef	NON_DESTR_TABS
	nextstop = (Column & (~7)) + 8;
	while (nextstop <= col)
	{
		putchar ('\t');
		Column = nextstop;
		nextstop = (Column & (~7)) + 8;
	}
#endif	NON_DESTR_TABS

	while (Column < col)
	{
		putchar (New[Column]);
		Column++;
	}

}	/* end cmove */

/*
 ****************************************************************
 *	Calcula a Coluna ocupada por line[order]		*
 ****************************************************************
 */
static int	
column (int order)
{
	register char	*p;
	register char	*End;
	register int	col;

#ifdef	DEBUG
	fprintf (stderr, " column: order=%d\n", order);
#endif	DEBUG

	col = First;

	for (p = Line, End = Line + order; *p && p <= End; p++)
	{
		if (isgraph (*p) || *p == ' ')
			col++;
		elif (*p == '\t')
			col = (col & ~7) + 8;
		else
			col += 2;
	}

	return (col-1);

}	/* end column */

/*
 ****************************************************************
 *	Campainha						*
 ****************************************************************
 */
static void	
beep (void)
{
	putchar ('\007');

}	/* end beep */

/*
 ****************************************************************
 *	Escreve Prompt e Line no Video. 			*
 ****************************************************************
 */
static void
display (register int n)	/* "n": Nova ordem apos escrever a linha */
{
	twrite (New);

	printf ("\r\n");

	for (Column = 0; New[Column] != '\0'; Column++)
		putchar (New[Column]);

	cmove (column (n));

	Order = n;

}	/* end display */

/*
 ****************************************************************
 *	Escreve Prompt e Line por extenso em linha.		*
 ****************************************************************
 */
static void	
twrite (register char *linha)
{
	register const  char	*s1, *End;
	register char		*s2;
	register int		n;

	End = linha + MAX;

	/*
	 *	Escreve Prompt
	 */
	for (s1 = Prompt, s2 = linha; s2 < End && *s1; s1++)
	{
		if (isgraph (*s1) || *s1 == ' ')
		{
			*s2++ = *s1;
		}
		elif (*s1 == '\t')
		{
			for (n = 8 - ((s2 - linha) & 7); n > 0; --n)
				*s2++ = ' ';
		}
		else
		{
			*s2++ = '^';
			*s2++ = (*s1 == DEL) ? DELCH : *s1 | 0100;
		}
	}

	First = s2 - linha;

#ifdef	DEBUG
	fprintf (stderr, " twrite: First=%d\n", First);
#endif	DEBUG

	/*
	 *	Escreve Line
	 */
	for (s1 = Line; s2 < End && *s1; s1++)
	{
		if (isgraph (*s1) || *s1 == ' ')
		{
			*s2++ = *s1;
		}
		elif (*s1 == '\t')
		{
			for (n = 8 - ((s2 - linha) & 7); n > 0; --n)
				*s2++ = ' ';
		}
		else
		{
			*s2++ = '^';
			*s2++ = (*s1 == DEL) ? DELCH : *s1 | 0100;
		}
	}
	*s2 = '\0';

}	/* end twrite */

/*
 ****************************************************************
 *	Reescreve New no Video a partir de Old.			*
 ****************************************************************
 */
static void	
redisplay (int n)	/* "n": Nova ordem */
{
	register int	col;
	register bool	podem_ser_iguais;
	register char	*new, *old;

#ifdef	DEBUG
	fprintf (stderr, "redisplay: o:%d\n", n);
#endif	DEBUG

	/*
	 *	Permuta Old e New
	 */
	old = Old;
	Old = New;
	New = old;

	/*
	 *	Atualiza New
	 */
	twrite (New);

	new = New;
	old = Old;

	/*
	 *	Pula parte inicial em que new e old são iguais
	 */
	for (col = 0; *new && *new == *old; col++, new++, old++)
		/* vazio */;

	/*
	 *	Retorna se não for necessario reescrever
	 */
	if (! *new  &&  ! *old)
		goto end_redisplay;
	if (! *new  &&  space (old))
		goto end_redisplay;
	if (! *old  &&  space (new))
		goto end_redisplay;

	/*
	 *	Posiciona o Cursor no inicio da parte diferente
	 */
	cmove (col);

	/*
	 *	Reescreve a parte diferente
	 */
	podem_ser_iguais = (strlen (new) == strlen (old));

	for (/* vazio */; *new; new++)
	{
		if
		(	(*old == '\0' && space (new)) ||
			(podem_ser_iguais && streq (new, old))
		)
		{
			Column = col;
			goto end_redisplay;
		}

		putchar (*new);
		col++;
		if (*old)
			old++;
	}

	/*
	 *	Caso new seja mais curta que old,
	 *	apague final de old
	 */
	for (/* vazio */; *old; col++ , old++)
		putchar (' ');

	Column = col;

    end_redisplay:
	if (Line[n] == '\0')
		n--;
	cmove (column (n));
	Order = n;

}	/* end redisplay */

/*
 ****************************************************************
 *	Verifica se cadeia é composta apenas de brancos		*
 ****************************************************************
 */
static bool
space (register const char *cadeia)
{
	for (/* vazio */; *cadeia == ' ' || *cadeia == '\t'; cadeia++)
		/* vazio */;

	return (*cadeia == '\0');

}	/* end space */
