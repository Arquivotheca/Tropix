/*
 ****************************************************************
 *								*
 *			xedit.c					*
 *								*
 *	Editor de textos para o X Windows			*
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
static XtActionsRec	actions[] = { {"quit", DoQuit} };
static Atom		wm_delete_window;
Widget			textwindow, messwidget, labelwindow, filenamewindow;
Display			*CurDpy;
struct _app_resources	app_resources;

#define	Offset(field)	XtOffsetOf(struct _app_resources, field)

static XtResource	resources[] =	{	{	"enableBackups",
							"EnableBackups",
							XtRBoolean,
							sizeof (Boolean),
							Offset (enableBackups),
							XtRImmediate,
							FALSE
						},
						{	"backupNamePrefix",
							"BackupNamePrefix",
							XtRString,
							sizeof (char *),
							Offset (backupNamePrefix),
							XtRString,
							""
						},
						{	"backupNameSuffix",
							"BackupNameSuffix",
							XtRString,
							sizeof (char *),
							Offset (backupNameSuffix),
							XtRString,
							".BAK"
						}
					};

#undef	Offset

/*
 ****** Protótipos de funções ***********************************
 */
void		ResetSourceChanged ();
static void	makeButtonsAndBoxes (Widget, char *);
void		Feep (void);

/*
 ****************************************************************
 *	Editor de textos para o X Windows			*
 ****************************************************************
 */
void
main (int argc, char *argv[])
{
	XtAppContext	appcon;
	Widget		top;
	String		filename = NULL;

	XtSetLanguageProc (NULL, NULL, NULL);
	top = XtAppInitialize (&appcon, "Xedit", NULL, 0, &argc, argv, NULL, NULL, 0);

	XtAppAddActions (appcon, actions, XtNumber(actions));

	XtOverrideTranslations (top, XtParseTranslationTable ("<Message>WM_PROTOCOLS: quit()"));


	XtGetApplicationResources (top, (XtPointer) &app_resources, resources, XtNumber (resources), NULL, 0);

	CurDpy = XtDisplay (top);

	if (argc > 1)
	{
		Boolean		exists;

		filename = argv[1];

		switch (CheckFilePermissions (filename, &exists))
		{
		    case NO_READ:
			if (exists)
			{
				msg
				(	"$Arquivo %s existe, mas não pode ser aberto para leitura.\n", 
					filename
				);
			}
			else
			{
				msg
				(	"$Arquivo %s não existe, "
					"e o diretório não pode ser aberto para escrita.\n",
					filename
				);
			}

		    case READ_OK:

		    case WRITE_OK:
			makeButtonsAndBoxes (top, filename);
			break;

		    default:
			msg ("$Função interna MaybeCreateFile() retornou um valor inesperedo.\n");

		}	/* end switch */

	}	/* end if */
	else
		makeButtonsAndBoxes (top, NULL);

	XtRealizeWidget (top);
	XDefineCursor (XtDisplay (top), XtWindow (top), XCreateFontCursor (XtDisplay (top), XC_left_ptr));

	wm_delete_window = XInternAtom (XtDisplay(top), "WM_DELETE_WINDOW", False);
	XSetWMProtocols (XtDisplay(top), XtWindow(top), &wm_delete_window, 1);

	XtAppMainLoop(appcon);

}	/* end xedit */

/*
 ****************************************************************
 *	Cria os botões e caixas					*
 ****************************************************************
 */
static void
makeButtonsAndBoxes (Widget parent, char *filename)
{
	Widget		outer, b_row;
	Arg		arglist[10];
	Cardinal	num_args;

	outer = XtCreateManagedWidget ("paned", panedWidgetClass, parent, NULL, ZERO);
 
	b_row = XtCreateManagedWidget ("buttons", panedWidgetClass, outer, NULL, ZERO);
	{
		MakeCommandButton (b_row, "quit", DoQuit);
		MakeCommandButton (b_row, "save", DoSave);
		MakeCommandButton (b_row, "load", DoLoad);
		(Widget) filenamewindow = MakeStringBox (b_row, "filename", filename); 
	}

	XtCreateManagedWidget ("bc_label", labelWidgetClass, outer, NULL, ZERO);

	num_args = 0;
	XtSetArg (arglist[num_args], XtNeditType, XawtextEdit);
	num_args++;
	messwidget = XtCreateManagedWidget ("messageWindow", asciiTextWidgetClass, outer, arglist, num_args);

	num_args = 0;

	if (filename != NULL) 
	{
		XtSetArg (arglist[num_args], XtNlabel, filename);
		num_args++;
	}

	labelwindow = XtCreateManagedWidget ("labelWindow", labelWidgetClass, outer, arglist, num_args);

	num_args = 0;
	XtSetArg (arglist[num_args], XtNtype, XawAsciiFile);
	num_args++;
	XtSetArg (arglist[num_args], XtNeditType, XawtextEdit);
	num_args++;
	textwindow = XtCreateManagedWidget ("editWindow", asciiTextWidgetClass, outer, arglist, num_args);

	if (filename != NULL)
		DoLoad();
	else
		ResetSourceChanged(textwindow);

}	/* end makeButtonsAndBoxes */

/*
 ****************************************************************
 *	Feep the bell						*
 ****************************************************************
 */

void
Feep ()
{
	XBell (CurDpy, 0);
}
