/*
 ****************************************************************
 *								*
 *			util.c					*
 *								*
 *	Utilitários do programa Xedit				*
 *								*
 *	Versão	3.2.1, de 21.10.99				*
 *								*
 *	Módulo: Xedit						*
 *		Utilitário para o sistema X Windows		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>		/* for types.h */
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>
#include <sys/stat.h>
#include "../h/xedit.h"

/*
 ****** Variáveis e Definições globais **************************
 */
extern Widget	messwidget;

/*
 ****** Protótipos de funções ***********************************
 */
void		XeditPrintf (char *);
Widget		MakeCommandButton (Widget, char *, XtCallbackProc);
Widget		MakeStringBox (Widget, String, String);
String		GetString (Widget);
FileAccess	MaybeCreateFile (char *);
FileAccess	CheckFilePermissions (char *, Boolean *);

void
XeditPrintf (char *str)
{
	XawTextBlock		text;
	static XawTextPosition	pos = 0;

	text.length = strlen (str);
	text.ptr = str;
	text.firstPos = 0;
	text.format = FMT8BIT;

	XawTextReplace (messwidget, pos, pos, &text);

	pos += text.length;
	XawTextSetInsertionPoint (messwidget, pos);
}

Widget
MakeCommandButton (Widget box, char *name, XtCallbackProc function)
{
	Widget w = XtCreateManagedWidget (name, commandWidgetClass, box, NULL, ZERO);

	if (function != NULL)
		XtAddCallback (w, XtNcallback, function, (caddr_t) NULL);

	return (w);
}

Widget 
MakeStringBox (Widget parent, String name, String string)
{
	Arg		args[5];
	Cardinal	numargs = 0;
	Widget		StringW;

	XtSetArg (args[numargs], XtNeditType, XawtextEdit);
	numargs++;
	XtSetArg (args[numargs], XtNstring, string);
	numargs++;

	StringW = XtCreateManagedWidget (name, asciiTextWidgetClass, parent, args, numargs);
	return (StringW);  
}
 
/*
 ****************************************************************
 *	Retorna a string de um Widget asciiText			*
 ****************************************************************
 */
String
GetString (Widget w)
{
	String		str;
	Arg		arglist[1];

	XtSetArg (arglist[0], XtNstring, &str);
	XtGetValues (w, arglist, ONE);
	return (str);
}

/*
 ****************************************************************
 *	Cria arquivo caso ele não exista			*
 ****************************************************************
 */
FileAccess
MaybeCreateFile (char *file)
{
	Boolean		exists;
	int		fd;

	if (access (file, F_OK) != 0)
	{
		fd = creat (file, 0666);
		if (fd != -1)
			close (fd);
	}

	return (CheckFilePermissions (file, &exists));
}

FileAccess
CheckFilePermissions (char *file, Boolean *exists)
{
	char temp[BUFSIZ], *ptr;

	if (access (file, F_OK) == 0)
	{
		*exists = TRUE;

		if (access (file, R_OK) != 0) 
			return (NO_READ);

		if (access (file, R_OK | W_OK) == 0) 
			return (WRITE_OK);

		return (READ_OK);
	}

	*exists = FALSE;

	strcpy (temp, file);

	if ((ptr = (char *) strrchr (temp, '/')) == NULL) 
		strcpy (temp, ".");
	else 
		*ptr = '\0';

	if (access (temp, R_OK | W_OK | X_OK) == 0)
		return (WRITE_OK);

	return (NO_READ);
}
