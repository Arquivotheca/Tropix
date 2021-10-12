/*
 ****************************************************************
 *								*
 *			diff2.c					*
 *								*
 *	Compara dois arquivos: saída 				*
 *								*
 *	Versão	2.3.0, de 25.01.90				*
 *		3.0.0, de 24.04.97				*
 *								*
 *	Módulo: diff						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stat.h>
#include <time.h>

#include "diff.h"

/*
 ****************************************************************
 *	Variáveis deste arquivo					*
 ****************************************************************
 */
static int	lastline;

/*
 ****************************************************************
 *	Imprime o resultado					*
 ****************************************************************
 */
void
output (void)
{
	int		old0, old1, new0, new1, lastoldline;

	lastoldline = oldlineno;

	J[0]   = 0;
	J[lastoldline+1] = newlineno + 1;

	/*
	 *	Percorre as linhas
	 */
	if (function == EDIT) for (old0 = lastoldline; old0 >= 1; old0 = old1-1)
	{
		/*
		 *	EDIT
		 */
		while (old0 >= 1  && J[old0] == J[old0+1] - 1  &&  J[old0] != 0)
			old0--;
		new0 = J[old0+1] - 1;
		old1 = old0 + 1;
		while (old1 > 1  &&  J[old1-1] == 0)
			old1--;
		new1 = J[old1-1] + 1;
		J[old1] = new1;
		change (old1, old0, new1, new0);
	}
	else for (old0 = 1; old0 <= lastoldline; old0 = old1 + 1)
	{
		/*
		 *	NORMAL
		 *	REVEDIT
		 *	CONTEXT
		 *	IFDEF
		 */
		while (old0 <= lastoldline  &&  J[old0] == J[old0-1] + 1)
			old0++;
		new0 = J[old0-1] + 1;
		old1 = old0 - 1;
		while (old1 < lastoldline  &&  J[old1+1] == 0)
			old1++;
		new1 = J[old1+1] - 1;
		J[old1] = new1;
		change (old0, old1, new0, new1);
	}

	/*
	 *	Se o "oldfile" for vazio
	 */
	if (lastoldline == 0)
		change (1, 0, 1, newlineno);

	/*
	 *	Emite o resto do arquivo "oldfile"
	 */
	if (function == IFDEF)
	{
		int	i;
		int		c;
		char	*cp;

		for (i = lastline + 1; i <= oldlineno; i++)
		{
			cp = oldpline[i].l_text;

			while ((c = *cp++) != '\n')
				putchar (c);

			fputc ('\n', stdout);
		}
	}

	/*
	 *	Providências finais
	 */
	if   (function == EDIT)
	{
		fputs ("w\nq\n", stdout);
	}
	elif (function == CONTEXT)
	{
		printf ("=======================================");
		printf ("=======================================");
		printf ("=\n");
	}

}	/* end output */

/*
 ****************************************************************
 *	Emite uma mensagem de mudança				*
 ****************************************************************
 */
void
change (int old0, int old1, int new0, int new1)
{
	int		oldlow, oldup, newlow, newup, chr;
	int		last, i;
	int		inifdef;
	char		*cp;
	char		ch;

	if (function != IFDEF  &&  old0 > old1  &&  new0 > new1)
		return;

	/*
	 *	Na primeira vez, coloca o cabeçalho do CONTEXT
	 */
	if (anychange == 0)
	{
		anychange = 1;

		if (function == CONTEXT)
		{
			printf
			(	"<arquivo1>: \"%s\"	%s",
				oldnm, btime (&olds.st_mtime)
			);

			printf
			(	"<arquivo2>: \"%s\"	%s\n",
				newnm, btime (&news.st_mtime)
			);
		}

	}	/* end if (anychange == 0) */

	/*
	 *	Obtém o código da diferença
	 */
	if (old0 <= old1  &&  new0 <= new1)
		ch = 'c';
	else
		ch = (old0 <= old1) ? 'd' : 'a';

	/*
	 *	Verifica qual a operação
	 */
	switch (function)
	{
		/*
		 ******	Funções NORMAL + EDIT *******************
		 */
	    case NORMAL:
	    case EDIT:
		range (old0, old1, ",");
		chr = (old0 > old1 ? 'a' : new0 > new1 ? 'd' : 'c');
		putchar (chr);
		if (function == NORMAL)
			range (new0, new1, ",");
		fputc ('\n', stdout);

		if (function == NORMAL)
		{
			fetch (oldpline, old0, old1, "< ");
			if (old0 <= old1  &&  new0 <= new1)
				fputs ("---\n", stdout);
			fetch (newpline, new0, new1, "> ");
		}
		else	/* EDIT */
		{
			fetch (newpline, new0, new1, "");
			if (new0 <= new1)
				fputs (".\n", stdout);
		}
		break;

		/*
		 ******	Função REVEDIT **************************
		 */
	    case REVEDIT:
		chr = (old0 > old1 ? 'a' : new0 > new1 ? 'd' : 'c');
		putchar (chr);
		range (old0, old1, " ");
		fputc ('\n', stdout);
		fetch (newpline, new0, new1, "");
		if (new0 <= new1)
			fputs (".\n", stdout);
		break;

		/*
		 ******	Função CONTEXT **************************
		 */
	    case CONTEXT:
		oldlow = max (1,	 old0 - context);
		oldup  = min (oldlineno, old1 + context);
		newlow = max (1,	 new0 - context);
		newup  = min (newlineno, new1 + context);

		printf ("=======================================");
		printf ("=======================================");
		printf ("=\n\n");

		printf ("------- <arquivo1>: \"%s\"	(", oldnm);
		range (oldlow, oldup, ",");
		printf (")\n\n");

		if (ch == 'a')
		{
			fetch (oldpline, oldlow, oldup, "  ");
		}
		else
		{
			fetch (oldpline, oldlow, old0-1, "  ");
			cp = (ch == 'c') ? "! " : "- ";
			fetch (oldpline, old0,   old1,   cp);
			fetch (oldpline, old1+1, oldup,  "  ");
		}

		printf ("\n------- <arquivo2>: \"%s\"	(", newnm);
		range (newlow, newup, ",");
		printf (")\n\n");

		if (ch == 'd')
		{
			fetch (newpline, newlow, newup, "  ");
		}
		else
		{
			fetch (newpline, newlow, new0-1, "  ");
			cp = (ch == 'c') ? "! " : "+ ";
			fetch (newpline, new0,   new1,   cp);
			fetch (newpline, new1+1, newup,  "  ");
		}

		printf ("\n\n");
		break;

		/*
		 ******	Função IFDEF ****************************
		 */
	    case IFDEF:
		last = (old0 <= old1) ? old0 - 1 : old1;

		for (i = lastline + 1; i <= last; i++)  
		{
			cp = oldpline[i].l_text;

			while ((chr = *cp++) != '\n')
				putchar (chr);

			fputc ('\n', stdout);
		}

		if (last > lastline)
			lastline = last;

		inifdef = 0;

		if (new0 <= new1)
		{
			printf  ("#ifdef	%s\n", ifdefname);

			fetch (newpline, new0, new1, "");

			inifdef = 1;
		}

		if (old0 <= old1)
		{
			if   (inifdef)
				printf  ("#else\n");
			else
				printf  ("#ifndef	%s\n", ifdefname);
	
			fetch (oldpline, old0, old1, "");

			inifdef = 1;
		}

		if (inifdef)
			printf ("#endif	%s\n", ifdefname);

		break;

	}	/* end switch (function) */

}	/* end change */

/*
 ****************************************************************
 *	Emite as linhas de uma região				*
 ****************************************************************
 */
void
range (int line0, int line1, char *separator)
{
	printf ("%d", line0 > line1 ? line1 : line0);

	if (line0 < line1)
		printf ("%s%d", separator, line1);

}	/* end range */

/*
 ****************************************************************
 *	Imprime um linha					*
 ****************************************************************
 */
void
fetch (PLINE *pline, int line0, int line1, char *msg)
{
	int		i, c;
	char		*cp;

	/*
	 *	Neste caso, simplemente não há linhas
	 */
	if (line0 > line1)
		return;

	/*
	 *	Emite as linhas em questão
	 */
	for (i = line0; i <= line1; i++)
	{
		cp = pline[i].l_text;

		if (function != IFDEF)
			fputs (msg, stdout);

		while ((c = *cp++) != '\n')
			putchar (c);

		fputc ('\n', stdout);
	}

	/*
	 *	Não esquece de guardar a última linha emitida do oldfile
	 */
	if (pline == oldpline) 
		lastline = line1;

}	/* end fetch */

/*
 ****************************************************************
 *	Acha o mínimo						*
 ****************************************************************
 */
int
min (int a, int b)
{
	return (a < b ? a : b);

}	/* end min */

/*
 ****************************************************************
 *	Acha o máximo						*
 ****************************************************************
 */
int
max (int a, int b)
{
	return (a > b ? a : b);

}	/* end max */
