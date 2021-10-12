/*
 ****************************************************************
 *								*
 *			h/feature.h				*
 *								*
 *	Opções de Compilação ligadas ao Funcionamento		*
 *								*
 *	Versão	3.0.0, de 17.09.97				*
 *		4.8.0, de 18.03.05				*
 *								*
 *	Módulo: programs/xterm					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2005 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

/*----------------------------------------------------------------------*
 * #define NDEBUG
 *	to disable whichever assert() macros are used in the code
 *
 * #define DEBUG_SCREEN
 *	to do self-check for internal screen corruption
 *
 * #define DEBUG_CMD
 *	to output some data structures of command.c
 *	(key-buffer contents, command line, tty permissions)
 *
 * #define DEBUG_TTY
 *	to output tty settings
 *
 * #define DEBUG_COLORS
 *	to print out current color/renditions as they change
 *
 * #define DEBUG_SELECTION
 *	to use XK_Print to dump information about the current selection
 *
 * #define DEBUG_DEPTH	1
 *  to set the X color depth, for debugging lower depth modes
 *----------------------------------------------------------------------*/
/* #define NDEBUG */
#ifndef NDEBUG
/* # define DEBUG_MENU */
/* # define DEBUG_MENUARROWS */
/* # define DEBUG_SCREEN */
/* # define DEBUG_CMD */
/* # define DEBUG_TTY */
/* # define DEBUG_COLORS */
/* # define DEBUG_SELECTION */
/* # define DEBUG_DEPTH 1 */
#endif

/*----------------------------------------------------------------------*
 * #define PATH_ENV "RXVTPATH"
 *	to define the name of the environment variable to be used in
 *	addition to the "PATH" environment and the `path' resource
 *
 * #define NO_BRIGHTCOLOR
 *	to suppress use of BOLD and BLINK attributes for setting
 *	bright foreground and background, respectively.
 *	Simulate BOLD using colorBD, boldFont or overstrike characters.
 *
 * #define NO_BOLDUNDERLINE
 *	to disable separate colors for bold/underline
 *
 * #define NO_BOLDOVERSTRIKE
 *	to disable using simulated bold using overstrike
 *
 * #define NO_BOLDFONT
 *	to compile without support for real bold fonts
 *
 * #define NO_SECONDARY_SCREEN
 *	to disable the secondary screen ("\E[?47h" / "\E[?47l")
 *
 * #define REFRESH_PERIOD <num>
 *	to limit the number of screenfulls between screen refreshes
 *	during hard & fast scrolling [default: 1]
 *
 * #define USE_XCOPYAREA
 *	to use XCopyArea (in place of re-draws) to speed up xterm.
 *	- I've been told this helps with some graphics adapters like the
 *	  PC's et4000. OK, it's good on monochrome Sun-3's that I've tried
 *	  too. /RN
 *	- sometimes looks worse and slower /mjo
 *
 * #define RXVT_GRAPHICS
 *	to use Rob Nation's own graphics mode
 *
 * #define OLD_COLOR_MODEL
 *	to use the old color model whereby erasing is done with the
 *	default rendition rather than the current rendition
 *	NB: this make break some applications and should used with caution
 *----------------------------------------------------------------------*/
#define PATH_ENV	"RXVTPATH"
/* #define NO_BRIGHTCOLOR */
/* #define NO_BOLDUNDERLINE */
#define NO_BOLDOVERSTRIKE
#define NO_BOLDFONT
/* #define NO_SECONDARY_SCREEN */
/* #define REFRESH_PERIOD	1 */
/* #define USE_XCOPYAREA */
/* #define RXVT_GRAPHICS */
/* #define OLD_COLOR_MODEL */

/*----------------------------------------------------------------------*
 * #define NO_RESOURCES
 *	to blow off the Xdefaults altogether
 *
 * #define USE_XGETDEFAULT
 *	to use XGetDefault() instead of the default, which is to use a
 *	substitute for using XGetDefaults() that saves 60-150kB memory
 *
 *	The default is best if all you want to do is put xterm defaults
 *	in ~/.Xdefaults file,
 *
 * #define XAPPLOADDIR	"/usr/lib/X11/app-defaults"
 *	to define where to find installed application defaults for xterm
 *	Only if USE_XGETDEFAULT is not defined.
 *
 * #define OFFIX_DND
 *	to add support for the Offix DND (Drag 'n' Drop) protocol
 *----------------------------------------------------------------------*/
/* #define NO_RESOURCES */
/* #define USE_XGETDEFAULT */
#ifndef XAPPLOADDIR
/* #define XAPPLOADDIR	"/usr/lib/X11/app-defaults" */
#endif
/* #define OFFIX_DND */

/*----------------------------------------------------------------------*
 * #define DONT_GUESS_BACKSPACE
 *	to use ^H for the Backspace key and avoid using the current stty
 *	setting of erase to guess a Backspace value of either ^H or ^?
 *
 * #define KEYSYM_RESOURCE
 *	to enable the keysym resource which allows you to define
 *	strings associated with various KeySyms (0xFF00 - 0xFFFF).
 *	Only works with the default hand-rolled resources.
 *----------------------------------------------------------------------*/
/* #define DONT_GUESS_BACKSPACE */
#define KEYSYM_RESOURCE

/*----------------------------------------------------------------------*
 * #define NO_SCROLLBAR_REPORT
 *	to disable sending escape sequences (up, down, page up/down)
 *	from the scrollbar when XTerm mouse reporting is enabled
 *
 * #define CUTCHAR_RESOURCE
 *	to add run-time support for changing the default cutchars
 *	for double click selection
 *
 * #define MOUSE_REPORT_DOUBLECLICK
 *	to have mouse reporting include double-click info for button1
 *
 * #define MULTICLICK_TIME <num>
 *	to set delay between multiple click events [default: 500]
 *----------------------------------------------------------------------*/
#define NO_SCROLLBAR_REPORT
/* #define CUTCHAR_RESOURCE */
/* #define MOUSE_REPORT_DOUBLECLICK */
/* #define MULTICLICK_TIME 500 */

/*----------------------------------------------------------------------*
 * #define NO_MAPALERT
 *	to disable automatic de-iconify when a bell is received
 *
 * #define MAPALERT_OPTION
 *	to have mapAlert behaviour selectable with mapAlert resource
 *----------------------------------------------------------------------*/
/* #define NO_MAPALERT */
/* #define MAPALERT_OPTION */

/*----------------------------------------------------------------------*
 * #define SCROLLBAR_RIGHT
 *	to have the scrollbar on the right-hand side
 *
 * #define SB_WIDTH	<width>
 *	to choose the scrollbar width - should be an even number [default: 8]
 *----------------------------------------------------------------------*/
#define SCROLLBAR_RIGHT
#define SB_WIDTH 10

/*----------------------------------------------------------------------*
 * #define ENABLE_DISPLAY_ANSWER
 *	to have "\E[7n" transmit the display name.
 *	This has been cited as a potential security hole.
 *
 * #define ESCZ_ANSWER	"\033[?1;2C"
 *	to change what ESC Z transmits instead of the default "\E[?1;2c"
 *
 * #define SMART_WINDOW_TITLE
 *	to check the current value of the window-time/icon-name and
 *	avoid re-setting it to the same value -- avoids unnecessary window
 *	refreshes
 *
 * #define XTERM_COLOR_CHANGE
 *	to allow foreground/background color to be changed with an
 *	xterm escape sequence "\E]39;color^G" -- still experimental
 *----------------------------------------------------------------------*/
/* #define ENABLE_DISPLAY_ANSWER */
/* #define ESCZ_ANSWER	"\033[?1;2C" */
/* #define SMART_WINDOW_TITLE */
/* #define XTERM_COLOR_CHANGE */

/*----------------------------------------------------------------------*
 * end of user configuration section
 *----------------------------------------------------------------------*/

#define APL_CLASS	"XTerm"	/* class name */
#define APL_SUBCLASS	"Xterm"	/* also check resources under this name */
#define APL_NAME	"xterm"	/* normal name */

/* COLORTERM, TERM environment variables */
#define COLORTERMENV	"xterm"
#define TERMENV		"xterm"

#ifdef NO_MOUSE_REPORT
# ifndef NO_MOUSE_REPORT_SCROLLBAR
#  define NO_MOUSE_REPORT_SCROLLBAR
# endif
#endif

#if defined (NO_RESOURCES) || defined (USE_XGETDEFAULT)
# undef KEYSYM_RESOURCE
#endif
#ifdef NO_RESOURCES
# undef USE_XGETDEFAULT
#endif
