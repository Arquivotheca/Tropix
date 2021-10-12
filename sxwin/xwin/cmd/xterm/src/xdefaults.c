/*
 ****************************************************************
 *								*
 *			src/xdefaults.c				*
 *								*
 *	Processa o arquivo $HOME/.Xdefaults			*
 *								*
 *	Versão	3.0.0, de 13.06.97				*
 *		4.5.0, de 18.08.03				*
 *								*
 *	Módulo: cmd/xterm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <X11/Xlib.h>

#include "../h/feature.h"
#include "../h/command.h"
#include "../h/misc.h"
#include "../h/main.h"
#include "../h/screen.h"
#include "../h/scrollbar.h"
#include "../h/xdefaults.h"

const char	*rs_title = NULL;		/* title name for window */
const char	*rs_geometry = NULL;	/* window geometry */
const char	*rs_saveLines = NULL;	/* scrollback buffer [lines] */

#ifdef KEYSYM_RESOURCE
const char	*KeySym_map [256];	/* probably mostly empty */
#endif

#if (0)	/*******************************************************/
static const char	*rs_loginShell = NULL;
static const char	*rs_utmpInhibit = NULL;
static const char	*rs_scrollBar = NULL;
static const char	*rs_visualBell = NULL;

#if !defined (NO_MAPALERT) && defined (MAPALERT_OPTION)
static const char	*rs_mapAlert = NULL;
static const char	*rs_reverseVideo = NULL;
#endif
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Tabela de opções/recursos				*
 ****************************************************************
 */
/* INFO() - descriptive information only */
#define INFO(opt,arg,desc)       {0, NULL, NULL, opt, arg, desc}

/* STRG() - command-line option, with/without resource */
#define STRG(p,kw,opt,arg,desc)  {0, &p, kw, opt, arg, desc}

/* RSTRG() - resource/long-option */
#define RSTRG(p,kw,arg)          {0, &p, kw, NULL, arg, NULL}

/* BOOL() - regular boolean `-/+' flag */
#define BOOL(p,kw,opt,flag,desc) {(Opt_Boolean|flag), &p, kw, opt, NULL, desc}

/* SWCH() - `-' flag */
#define SWCH(opt,flag,desc)      {(flag), NULL, NULL, opt, NULL, desc}

/* convenient macros */
#define optList_strlen(i)	(optList[i].flag			\
				    ? 0					\
				    : (optList[i].arg			\
					    ? strlen (optList[i].arg)	\
					    : 1				\
				      )					\
				)
#define optList_isBool(i)	(optList[i].flag & Opt_Boolean)
#define optList_size()		(sizeof(optList)/sizeof(optList[0]))

static const struct
{
	ulong			flag;
	const char		**dp;	/* data pointer */
	const char * const	kw;	/* keyword */
	const char * const	opt;	/* option */
	const char * const	arg;	/* argument */
	const char * const	desc;	/* description */

}	optList[] =
{
	STRG
	(	display_name,		NULL,
		"d",			"<servidor>",
		"servidor X a contactar"
	),

	STRG (display_name, NULL, "display", NULL, NULL),

	STRG
	(	desktop_number,		NULL,
		"D",			"<número>",
		"Desktop onde será criada a janela"
	),

	STRG (desktop_number, NULL, "desktop", NULL, NULL),

	STRG
	(	rs_geometry,		"geometry",
		"g",			"<geometria>",
		"tamanho (em caracteres) e posição"
	),

	STRG (rs_geometry, NULL, "geometry", NULL, NULL),

	STRG
	(	rs_color [bgColor],	"background",
		"b",			"<fundo>",
		"cor de fundo"
	),

	STRG (rs_color[bgColor], NULL, "bg", NULL, NULL),

	STRG
	(	home_directory,		"home",
		"h",			"<dir>",
		"diretório inicial"
	),

	STRG (home_directory, NULL, "home", NULL, NULL),

	STRG
	(	rs_color [fgColor],	"foreground",
		"f",			"<frente>",
		"cor principal"
	),

	STRG (rs_color[fgColor], NULL, "fg", NULL, NULL),

	RSTRG (rs_color [scrollColor], "scrollColor", "color"),

	STRG
	(	rs_color[minColor+0],	"color0",
		"color0",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+1],	"color1",
		"color1",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+2],	"color2",
		"color2",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+3],	"color3",
		"color3",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+4],	"color4",
		"color4",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+5],	"color5",
		"color5",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+6],	"color6",
		"color6",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+7],	"color7",
		"color7",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+8],	"color8",
		"color8",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+9],	"color9",
		"color9",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+10],	"color10",
		"color10",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+11],	"color11",
		"color11",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+12],	"color12",
		"color12",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+13],	"color13",
		"color13",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+14],	"color14",
		"color14",		NULL,
		NULL
	),

	STRG
	(	rs_color[minColor+15],	"color15",
		"color15",		NULL,
		NULL
	),

#ifndef NO_BOLDFONT
	STRG
	( 	rs_boldFont,		"boldFont",
		"fb",			"fontname",
		"fonte para negrito"
	),
#endif
	STRG
	( 	rs_font [0],		"font",
		"F",			"<font>",
		"fonte para texto normal"
	),

	STRG (rs_font [0], NULL, "fn", NULL, NULL),

	SWCH ("i",	Opt_iconic, "comece minimizado"),

	STRG
	( 	rs_name,		NULL,
		"n",			"<nome>",
		"nome genérico (título, ícone, ...)"
	),

	STRG (rs_name, NULL, "name", NULL, NULL),

	STRG
	( 	rs_title,		"title",
		"t",			"<título>",
		"título da janela"
	),

	STRG (rs_title, NULL, "title", NULL, NULL),

	STRG
	( 	rs_color [cursorColor],	"cursorColor",
		"c",			"<cor>",
		"cor do cursor"
	),

	STRG (rs_color [cursorColor], NULL, "cr", NULL, NULL),

	/* command-line option = resource name */
	RSTRG (rs_color [cursorColor2], "cursorColor2", "color"),

	STRG
	( 	rs_saveLines,		"saveLines",
		"s",			"<linhas>",
		"número de linhas roladas a salvar"
	),

#ifdef CUTCHAR_RESOURCE
	RSTRG (rs_cutchars, "cutchars", "string"),
#endif	/* CUTCHAR_RESOURCE */

	SWCH ("C", Opt_console, "captura mensagens do kernel"),
};

#undef INFO
#undef STRG
#undef RSTRG
#undef SWCH
#undef BOOL

/*
 ****************************************************************
 *	Ensina a usar o programa				*
 ****************************************************************
 */
static void
usage (int type)
{
   int i, col;
#define INDENT 25

	fprintf
	(	stderr,
		"%s - Emulador de terminal ANSI para X-Window\n\n"
		"%s\n\n"
		"Sintaxe:\n\t%s",
		pgname,	pgversion, pgname
	);

   switch (type) {
    case 0:			/* brief listing */
      fprintf (stderr, " [-help]\n   ");
      col = 3;
      for (i = 0; i < optList_size (); i++)
	{
	   if (optList[i].desc != NULL)
	     {
		int len = 2;
		if (!optList_isBool (i))
		  {
		     len = optList_strlen (i);
		     if (len > 0)
		       len++;		/* account for space */
		  }
		len += 4 + strlen (optList[i].opt);

		col += len;
		if (col > 79)		/* assume regular width */
		  {
		     fprintf (stderr, "\n   ");
		     col = 3 + len;
		  }
		fprintf (stderr, " [-");
		if (optList_isBool (i))
		  fprintf (stderr, "/+");
		fprintf (stderr, "%s", optList[i].opt);
		if (optList_strlen (i))
		  fprintf (stderr, " %s]", optList[i].arg);
		else
		  fprintf (stderr, "]");
	     }
	}
      fprintf (stderr, "\n\n");
      break;

    case 1:			/* full command-line listing */
      fprintf (stderr,
	       " [opções] [-e linha de comando]\n\n"
	       "onde opções pode ser:\n");

      for (i = 0; i < optList_size (); i++)
	if (optList[i].desc != NULL)
	  fprintf (stderr, "\t%s%s %-*s%s%s\n",
		   (optList_isBool (i) ? "-/+" : "-"),
		   optList[i].opt,
		   (INDENT - strlen (optList[i].opt)
		    + (optList_isBool (i) ? 0 : 2)),
		   (optList[i].arg ? optList[i].arg : ""),
		   (optList_isBool (i) ? "liga/desliga " : ""),
		   optList[i].desc);
      break;
   }
   exit (1);
}

/*
 ****************************************************************
 *	Obtém as opções da linha de comando			*
 ****************************************************************
 */
void
get_options (int argc, const char *argv[])
{
	int				i, bad_option = 0;
	static const char * const	On = "ON";
	static const char * const	Off = "OFF";

	for (i = 1; i < argc; i++)
	{
		int		ent, longopt = 0;
		const char	*flag;
		const char	*opt = argv [i];

		if (*opt == '-')
		{
			flag = On;
			if (*++opt == '-')
				longopt = *opt++;   /* long option */
		}
		else if (*opt == '+')
		{
			flag = Off;
			if (*++opt == '+')
				longopt = *opt++;   /* long option */
		}
		else
		{
			bad_option = 1;
			msg ("bad option \"%s\"", opt);
			continue;
		}

	        if (streq (opt, "help") || streq (opt, "H"))
			usage (1);

		/* feature: always try to match long-options */
	        for (ent = 0; ent < optList_size (); ent++)
		{
			if
			(	(optList[ent].kw && streq (opt, optList[ent].kw)) ||
				(!longopt && optList[ent].opt && streq (opt, optList[ent].opt))
			)
				break;
		}

		if (ent < optList_size ())
		{
			if (optList_strlen (ent))	/* string value */
			{
				const char	*str = argv [++i];

				if (flag == On && str && optList [ent].dp)
				{
					*(optList[ent].dp) = str;

					/* special cases */
					if (streq (opt, "n"))
					{
						if (!rs_title)
							rs_title = str;
					}
					else
					{
						if (streq (opt, "name"))
						{
							if (!rs_title)
								rs_title = str;
						}
					}
				}
			}
			else			/* boolean value */
			{
				if (flag == On)
					Options |= (optList[ent].flag);
				else
					Options &= ~(optList[ent].flag);

				if (optList [ent].dp)
					*(optList [ent].dp) = flag;
			}
		}
		else
			bad_option = 1;
	}

	if (bad_option)
		usage (0);

}	/* end get_options */

#ifndef NO_RESOURCES
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
static int
my_strcasecmp (const char *s1, const char *s2)
{
	for (/* vazio */; (*s1 && *s2); s1++, s2++)
	{
		int	c1 = toupper (*s1);
		int	c2 = toupper (*s2);

		if (c1 != c2)
			return (c1 - c2);
	}

	return ((int)(*s1 - *s2));

}	/* end my_strcasecmp */

#ifndef USE_XGETDEFAULT
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * remove leading/trailing space and strip-off leading/trailing quotes
 */
static char *
trim_string (char * str)
{
	int	n;

	if (str && *str)
	{
		while (*str && isspace (*str))
			str++;

		for (n = strlen (str) - 1; n > 0 && isspace (str [n]); n--)
			/* vazio */;

		str [n+1] = '\0';

		/* strip leading/trailing quotes */
		if (str [0] == '"')
		{
			str++; n--;
			if (str [n] == '"')
				str [n--] = '\0';
		}

		if (n < 0)
			str = NULL;
	}

	return (str);

}	/* end trim_string */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * the matching algorithm used for memory-save fake resources
 */
static void
get_xdefaults (FILE * stream, const char *name)
{
	unsigned int	len;
	char		*str, buffer [256];

	if (stream == NULL)
		return;

	len = strlen (name);
	while ((str = fgets (buffer, sizeof(buffer), stream)) != NULL)
	{
		unsigned int	ent, n;

		while (*str && isspace (*str))
			str++;			/* leading whitespace */

		if
		(	(str [len] != '*' && str [len] != '.') ||
			(len && strncmp (str, name, len))
		)
			continue;

		str += (len + 1);	/* skip `name*' or `name.' */

		/*
		 * look for something like this (XK_Delete)
		 * xterm*keysym.0xFFFF: "\177"
		 */
# ifdef KEYSYM_RESOURCE
		n = strlen ("keysym");
		if (str [n] == '.' && !strncmp (str, "keysym", n))
		{
			int	sym;
			str += (n + 1);	/* skip `keysym.' */

			/* some scanf() have trouble with a 0x prefix */
			if (str [0] == '0' && toupper (str [1]) == 'X')
				str += 2;

			if (sscanf (str, "%x:", &sym) == 1)
			{
				/* cue to ':', it's there since sscanf() worked */
				str = strchr (str, ':');
				str = trim_string (str + 1);
				n = escaped_string (str);

				/* only do extended keys */
				if (sym >= 0xFF00)
					sym -= 0xFF00;

				if (sym < 0 || sym > 0xFF)
					continue;

				if (n >= 256)
					n = 255;

		 		if (n && KeySym_map [sym] == NULL)	/* not already set */
				{
					char	*p;

					p = xtmalloc ((n+1) * sizeof(char));
					p [0] = n;
					memcpy (p+1, str, n);
					KeySym_map [sym] = p;
				}
			}
		}
		else
# undef KEYSYM_kw
# endif	/* KEYSYM_RESOURCE */
			for (ent= 0; ent< optList_size (); ent++)
			{
				const char * const kw = optList[ent].kw;

				if (kw == NULL)
					continue;

				n = strlen (kw);
				if (str [n] == ':' && !strncmp (str, kw, n))
				{
					/* skip `keyword:' */
					str = trim_string (str + n + 1);
					n = (str ? strlen (str) : 0);

					if (n && *(optList[ent].dp) == NULL)
					{
						/* not already set */
						char * p = xtmalloc ((n+1) * sizeof(char));

						strcpy (p, str);
						*(optList[ent].dp) = p;
						if (optList_isBool (ent))
						{
							if (!my_strcasecmp (str, "TRUE"))
								Options |= (optList[ent].flag);
							else
								Options &= ~(optList[ent].flag);
						}
					}

					break;
				}
			}
	}

	rewind (stream);

}	/* end get_xdefaults */
#endif	/* ! USE_XGETDEFAULT */
#endif	/* NO_RESOURCES */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
/*
 * using XGetDefault() or the hand-rolled replacement
 */
void
extract_resources (Display * display, const char * name)
{
#ifndef NO_RESOURCES
#ifdef USE_XGETDEFAULT
	/*
	 * get resources using the X library function
	 */
	int ent;
	for (ent= 0; ent< optList_size (); ent++)
	{
		char	*p;
		const char	*kw =  optList[ent].kw;

		if (kw == NULL || *(optList[ent].dp) != NULL)
			continue;	/* previously set */

		if
		(	(p = XGetDefault (display, name, kw)) != NULL ||
			(p = XGetDefault (display, APL_SUBCLASS, kw)) != NULL ||
			(p = XGetDefault (display, APL_CLASS, kw)) != NULL
		)
		{
			*optList[ent].dp = p;

			if (optList_isBool (ent))
			{
				if (!my_strcasecmp (p, "TRUE"))
					Options |= (optList[ent].flag);
				else
					Options &= ~(optList[ent].flag);
			}
		}
	}
#else	/* USE_XGETDEFAULT */
	/* get resources the hard way, but save lots of memory */
	const char	*fname[] = { ".Xdefaults", ".Xresources" };
	FILE		*fd = NOFILE;
	const char	*home;

	if ((home = getenv ("HOME")) != NOSTR)
	{
		int	i, len = strlen (home) + 2;
		char	*f = NULL;

		for (i = 0; i < (sizeof (fname) / sizeof (fname[0])); i++)
		{
			f = xtrealloc (f, (len + strlen (fname[i])) * sizeof(char));
			sprintf (f, "%s/%s", home, fname [i]);

			if ((fd = fopen (f, "r")) != NOFILE)
				break;
		}

		xtfree (f);
	}

	/*
	 * The normal order to match resources is the following:
	 * @ global resources (partial match, ~/.Xdefaults)
	 * @ application file resources (XAPPLOADDIR/RXvt)
	 * @ class resources (~/.Xdefaults)
	 * @ private resources (~/.Xdefaults)
	 *
	 * However, for the hand-rolled resources, the matching algorithm
	 * checks if a resource string value has already been allocated
	 * and won't overwrite it with (in this case) a less specific
	 * resource value.
	 *
	 * This avoids multiple allocation.  Also, when we've called this
	 * routine command-line string options have already been applied so we
	 * needn't to allocate for those resources.
	 *
	 * So, search in resources from most to least specific.
	 *
	 * Also, use a special sub-class so that we can use either or both of
	 * "XTerm" and "RXvt" as class names.
	 */

	get_xdefaults (fd, name);
	get_xdefaults (fd, APL_SUBCLASS);

#ifdef XAPPLOADDIR
{
	FILE	*ad;

	if ((ad = fopen (XAPPLOADDIR "/" APL_SUBCLASS, "r")) != NOFILE)
	{
		get_xdefaults (ad, "");
		fclose (ad);
	}
}
#endif	/* XAPPLOADDIR */

	get_xdefaults (fd, APL_CLASS);
	get_xdefaults (fd, "");		/* partial match */

	if (fd != NOFILE)
		fclose (fd);
#endif	/* USE_XGETDEFAULT */
#endif	/* NO_RESOURCES */
}
