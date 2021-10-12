/*
 ****************************************************************
 *								*
 *			src/misc.c				*
 *								*
 *	Um "saco de gatos"					*
 *								*
 *	Versão	3.0.0, de 31.05.97				*
 *		4.4.0, de 19.03.03				*
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

#include "../h/misc.h"
#include "../h/main.h"

/*
 ****************************************************************
 *	Interpreta seqüências especiais em uma cadeia		*
 ****************************************************************
 */
int
escaped_string (char * str)
{
	char	*p = str;
	int	i = 0, len, meta_x = 0;

	/* use 'i' to increment through destination and p through source */

	if (str == NULL || (len = strlen (str)) == 0)
		return (0);

	/* Emacs convenience, replace leading `M-..' with `\E..' */
	if (strncmp (p, "M-", 2) == 0)
	{
		str [i++] = '\033';		/* destination */
		p += 2;
		len--;

		if (toupper (*p) == 'X')
		{
			meta_x = 1;
			str [i++] = 'x';		/* destination */
			p++;

			while (isspace (*p))
				{ p++; len--; }
		}
	}

	for (/* vazio */; i < len; i++)
	{
		char	ch = *p++;

		if (ch == '\\')
		{
			ch = *p;
			if (ch >= '0' && ch <= '7')	/* octal */
			{
				int j, num = 0;
				for (j = 0; j < 3 && (ch >= '0' && ch <= '7'); j++)
				{
					num = num * 010 + (ch - '0');
					p++; len--;
					ch = *p;
				}

				ch = (uchar)num;
			}
			else
			{
				p++; len--;
				switch (ch)
				{
				    case 'a':
					ch = 007;
					break;	/* bell */
				    case 'b':
					ch = '\b';
					break;	/* backspace */
				    case 'E':
				    case 'e':
					ch =  033;
					break;	/* escape */
				    case 'n':
					ch = '\n';
					break;	/* newline */
				    case 'r':
					ch = '\r';
					break;	/* carriage-return */
				    case 't':
					ch = '\t';
					break;	/* tab */
				}
			}
		}
		else if (ch == '^')
		{
			ch = *p;
			p++; len--;
			ch = toupper (ch);
			ch = (ch == '?' ? 127 : (ch - '@'));
		}

		str [i] = ch;
	}

	/* add trailing carriage-return for `M-xcommand' */
	if (meta_x && str [len-1] != '\r')
		str [len++] = '\r';

	str [len] = '\0';

	return (len);

}	/* end escaped_string */

/*
 * draw bottomShadow/highlight along top/left sides of the window
 */
static void
Draw_tl (Window win, GC gc, int x, int y, int w, int h)
{
	int	shadow = SHADOW;

	if (w == 0 || h == 0)
		shadow = 1;

	w += (x - 1);
	h += (y - 1);

	for (/* vazio */; shadow > 0; shadow--, x++, y++, w--, h--)
	{
		XDrawLine (dpy, win, gc, x, y, w, y);
		XDrawLine (dpy, win, gc, x, y, x, h);
	}

}	/* end Draw_tl */

/*
 * draw bottomShadow/highlight along the bottom/right sides of the window
 */
static void
Draw_br (Window win, GC gc, int x, int y, int w, int h)
{
	int	shadow = SHADOW;

	if (w == 0 || h == 0)
		shadow = 1;

	w += (x - 1);
	h += (y - 1);

	for (x++, y++; shadow > 0; shadow--, x++, y++, w--, h--)
	{
		XDrawLine (dpy, win, gc, w, h, w, y);
		XDrawLine (dpy, win, gc, w, h, x, h);
	}

}	/* end Draw_br */

void
Draw_Shadow (Window win, GC topShadow, GC botShadow,
		int x, int y, int w, int h)
{
	Draw_tl (win, topShadow, x, y, w, h);
	Draw_br (win, botShadow, x, y, w, h);

}	/* end Draw_Shadow */

#if (0)	/*******************************************************/
/* button shapes */
void
Draw_Triangle (Window win, GC topShadow, GC botShadow,
		int x, int y, int w, int type)
{
	switch (type)
	{
	    case 'r':			/* right triangle */
		XDrawLine (dpy, win, topShadow, x, y, x, y + w);
		XDrawLine (dpy, win, topShadow, x, y, x + w, y + w/2);
		XDrawLine (dpy, win, botShadow, x, y + w, x + w, y + w/2);
		break;

	    case 'l':			/* right triangle */
		XDrawLine (dpy, win, botShadow, x + w, y + w, x + w, y);
		XDrawLine (dpy, win, botShadow, x + w, y + w, x, y + w/2);
		XDrawLine (dpy, win, topShadow, x, y + w/2, x + w, y);
		break;

	    case 'd':			/* down triangle */
		XDrawLine (dpy, win, topShadow, x, y, x + w/2, y + w);
		XDrawLine (dpy, win, topShadow, x, y, x + w, y);
		XDrawLine (dpy, win, botShadow, x + w, y, x + w/2, y + w);
		break;

	    case 'u':			/* up triangle */
		XDrawLine (dpy, win, botShadow, x + w, y + w, x + w/2, y);
		XDrawLine (dpy, win, botShadow, x + w, y + w, x, y + w);
		XDrawLine (dpy, win, topShadow, x, y + w, x + w/2, y);
		break;
#if 0
	    case 's':			/* square */
		XDrawLine (dpy, win, topShadow, x + w, y, x, y);
		XDrawLine (dpy, win, topShadow, x, y, x, y + w);
		XDrawLine (dpy, win, botShadow, x, y + w, x + w, y + w);
		XDrawLine (dpy, win, botShadow, x + w, y + w, x + w, y);
		break;
#endif
	}

}	/* end Draw_Triangle */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Aloca uma área						*
 ****************************************************************
 */
void *
xtmalloc (size_t size)
{
	void	*ptr;

	if ((ptr = malloc (size)) == NULL)
	{
		msg ("malloc error: size 0x%X\n", size);
		abort ();
	}

	return (ptr);

}	/* end xtmalloc */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Aloca e zera uma área					*
 ****************************************************************
 */
void *
xtcalloc (size_t count, size_t size)
{
	void	*ptr;

	if ((ptr = calloc (count, size)) == NULL)
	{
		msg ("calloc error: count * size 0x%X\n", count * size);
		abort ();
	}

	return (ptr);

}	/* end xtcalloc */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Redimensiona uma área					*
 ****************************************************************
 */
void *
xtrealloc (void *mem, size_t size)
{
	void	*ptr;

	if ((ptr = (mem ? realloc (mem, size) : malloc (size))) == NULL)
	{
		msg ("realloc error: size 0x%X\n", size);
		abort ();
	}

	return (ptr);

}	/* end xtrealloc */

/*
 ****************************************************************
 *	Libera uma área						*
 ****************************************************************
 */
void
xtfree (void *mem)
{
	if (mem != NULL)
		free (mem);

}	/* end xtfree */
