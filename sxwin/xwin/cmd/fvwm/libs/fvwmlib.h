/*
 ****************************************************************
 *								*
 *			fvwmlib.h				*
 *								*
 *	Definições acerca da biblioteca fvwmlib			*
 *								*
 *	Versão	1.0.0, de 27.08.97				*
 *								*
 *	Módulo: xc/programs/fvwm/libs				*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#ifndef FVWMLIB_H
#define FVWMLIB_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int	strcasecmp (char *, char *);
int	strncasecmp (char *, char *, int);
char	*CatString3 (char *, char *, char *);
void	SendInfo (int *, char *, ulong);
char	*safemalloc (int);
char	*findIconFile (char *, char *, int);
int	ReadFvwmPacket (int, ulong *, ulong **);
void	CopyString (char **, char *);
int	GetFdWidth (void);
void	*GetConfigLine (int *, char **);
void	SetMessageMask (int *, ulong);
int	envExpand (char *, int);
char	*envDupExpand (const char *, int);

typedef struct _Picture	Picture;

struct _Picture
{
	Picture		*next;
	char		*name;
	Pixmap		picture;
	Pixmap		mask;
	unsigned int	depth;
	unsigned int	width;
	unsigned int	height;
	unsigned int	count;
};

void		InitPictureCMap (Display *, Window);
Picture		*GetPicture (Display *, Window, char *, char *, char *);
Picture		*CachePicture (Display *, Window, char *, char *, char *);
void		DestroyPicture (Display *, Picture *);

XFontStruct	*GetFontOrFixed (Display *disp, char *fontname);

#endif
