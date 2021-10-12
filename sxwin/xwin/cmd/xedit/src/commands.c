/*
 ****************************************************************
 *								*
 *			command.c				*
 *								*
 *	Comandos do programa Xedit				*
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

#ifdef CRAY
#include <unistd.h>
#endif

extern Widget	textwindow, labelwindow, filenamewindow;
static Boolean	double_click = FALSE, source_changed = FALSE;

void		ResetSourceChanged();
static void	ResetDC();

void		AddDoubleClickCallback (Widget, Boolean);
static void	ResetDC (Widget, XtPointer, XtPointer);
char		*makeBackupName (String, String);
void		DoQuit (void);
void		DoSave (void);
void		DoLoad (void);
static void	SourceChanged (Widget, XtPointer, XtPointer);
void		ResetSourceChanged (Widget);

/*
 ****************************************************************
 *	Seta em double_click a necessidade de confirmar ações	*
 ****************************************************************
 */
void
AddDoubleClickCallback (Widget w, Boolean state)
{
	Arg			args[1];
	static XtCallbackRec	cb[] = { {NULL, NULL}, {NULL, NULL} };
 
	if (state) 
		cb[0].callback = ResetDC;
	else
		cb[0].callback = NULL;

	XtSetArg (args[0], XtNcallback, cb);
	XtSetValues (w, args, ONE);
}

/*
 ****************************************************************
 *	Reseta double_click					*
 ****************************************************************
 */
static void
ResetDC (Widget w, XtPointer junk, XtPointer garbage)
{
	double_click = FALSE;

	AddDoubleClickCallback(w, FALSE);
}

void
DoQuit (void)
{
	if (double_click || !source_changed) 
		exit (0); 

	XeditPrintf ("Alterações não salvas. Salve-as ou aperte Sair novamente.\n");
	Feep ();
	double_click = TRUE;
	AddDoubleClickCallback (textwindow, TRUE);
}

char *
makeBackupName (String buf, String filename)
{
	sprintf
	(	buf,
		"%s%s%s", app_resources.backupNamePrefix,
		filename, app_resources.backupNameSuffix
	);
	return (buf);
}

#if defined(USG) && !defined(CRAY)
int		rename (char *from, char *to);

int
rename (char *from, char *to)
{
	unlink (to);

	if (link (from, to) == 0)
	{
		unlink (from);
		return (0);
	}
	else
	{
		return (-1);
	}
}
#endif

void
DoSave (void)
{
	String filename = GetString (filenamewindow);
	char buf[BUFSIZ];

	if( (filename == NULL) || (strlen (filename) == 0) )
	{
		XeditPrintf ("Salvar:  nenhum arquivo especificado -- nada foi salvo\n");
		Feep();
		return;
	}

	if (app_resources.enableBackups)
	{
		char backup_file[BUFSIZ];
		makeBackupName (backup_file, filename);

		if (rename (filename, backup_file) != 0)
		{
			sprintf (buf, "erro backupeando arquivo:  %s\n",  backup_file); 
			XeditPrintf (buf);
		}
	}

	switch (MaybeCreateFile (filename))
	{
	    case NO_READ:

	    case READ_OK:
		sprintf (buf, "Arquivo %s não pode ser aberto para escrita.\n", filename);
		break;

	    case WRITE_OK:
		if (XawAsciiSaveAsFile (XawTextGetSource (textwindow), filename))
		{
			sprintf	(buf, "Arquivo salvo:  %s\n", filename);
			ResetSourceChanged (textwindow);
		}
		else 
		{
			sprintf (buf, "Erro salvando arquivo:  %s\n",  filename);
		}

		break;

	    default:
		sprintf
		(	buf, "Função interna MaybeCreateFile()"
			" retornou valor inesperado.\n"
		);
	}

	XeditPrintf (buf);

}	/* end DoSave */

void
DoLoad (void)
{
	Arg		args[5];
	Cardinal	num_args = 0;
	String		filename = GetString (filenamewindow);
	char		buf[BUFSIZ], label_buf[BUFSIZ];

	if (source_changed && !double_click)
	{
		XeditPrintf ("Alterações não salvas. Salve-as ou aperte Ler novamente.\n");
		Feep ();
		double_click = TRUE;
		AddDoubleClickCallback (textwindow, TRUE);
		return;
	}

	double_click = FALSE;

	if ((filename != NULL) && (strlen (filename) > 0))
	{
		Boolean exists;

		switch (CheckFilePermissions (filename, &exists))
		{
		    case NO_READ:
			if (exists)
			{
				sprintf
				(	buf,
					"Arquivo %s existe, mas não pode ser aperto para leitura.\n",
					filename
				);
			}
			else
			{
				sprintf
				(	buf,
					"Arquivo %s não existe, e "
					"não pude escrever no diretório.\n",
					filename
				);
			}

			XeditPrintf (buf);
			Feep ();
			return;

		    case READ_OK:
			XtSetArg (args[num_args], XtNeditType, XawtextRead);
			num_args++;
			sprintf (label_buf, "%s       SOMENTE LEITURA", filename);
			sprintf (buf, "Arquivo %s aberto para SOMENTE LEITURA.\n", filename);
			break;

		    case WRITE_OK:
			XtSetArg (args[num_args], XtNeditType, XawtextEdit);
			num_args++;
			sprintf (label_buf, "%s       Leitura - Escrita", filename);
			sprintf (buf, "Arquivo %s aberto para leitura - escrita.\n", filename);
			break;

		    default:
			sprintf
			(	buf, "Função interna MaybeCreateFile() "
				"retornou valor inesperado.\n"
			);
			XeditPrintf (buf);
			return;

		}	/* end shitch */

		XeditPrintf (buf);
	
		if (exists)
		{
			XtSetArg (args[num_args], XtNstring, filename);
			num_args++;
		}
		else
		{
			XtSetArg(args[num_args], XtNstring, NULL); num_args++;
		}

		XtSetValues( textwindow, args, num_args);

		num_args = 0;
		XtSetArg (args[num_args], XtNlabel, label_buf);
		num_args++;
		XtSetValues (labelwindow, args, num_args);
		ResetSourceChanged (textwindow);
		return;

	}	/* end if */

	XeditPrintf ("Ler: Nenhum arquivo especificado.\n");
	Feep ();
} /* end DoLoad */

/*
 ****************************************************************
 *	Rotina chamada quando a fonte é alterada		*
 ****************************************************************
 */
static void
SourceChanged (Widget w, XtPointer junk, XtPointer garbage)
{
	XtRemoveCallback (w, XtNcallback, SourceChanged, NULL);
	source_changed = TRUE;
}

/*
 ****************************************************************
 *	Seta a alteração como falso e a prepara para mudar	*
 ****************************************************************
 */
void
ResetSourceChanged (Widget widget)
{
	XtAddCallback (XawTextGetSource (widget), XtNcallback, SourceChanged, NULL);
	source_changed = FALSE;
}
