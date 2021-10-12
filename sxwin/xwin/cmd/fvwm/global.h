/*
 ****************************************************************
 *								*
 *			global.h				*
 *								*
 *	Configuração Geral para o Fvwm e seus Módulos		*
 *								*
 *	Versão	1.0.0, de 30.07.97				*
 *								*
 *	Módulo: programs/fvwm					*
 *		X Windows - Clientes				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1995 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#define HAVE_WAITPID		0
#define	HAVE_SYSCONF		0
#define HAVE_UNAME		1
#define	HAVE_GETHOSTNAME	0
#define HAVE_STRERROR		1

/*
 ****************************************************************
 *	Cadeias dependentes da língua				*
 ****************************************************************
 */
#define MOVE_STRING		"mover"
#define RESIZE_STRING1		"tamanho"
#define RESIZE_STRING2		"redimensionar"
#define MINIMIZE_STRING		"minimizar"
#define MINIMIZE_STRING2	"iconificar"
#define MAXIMIZE_STRING		"maximizar"
#define CLOSE_STRING1		"fechar"
#define CLOSE_STRING2		"apagar"
#define CLOSE_STRING3		"destruir"
#define CLOSE_STRING4		"sair"

#define	FVWM_INLINE	/* nada */

/*
 ****************************************************************
 *	Mensagens de Depuração					*
 ****************************************************************
 */
/* #define FVWM_DEBUG_MSGS */

#ifdef FVWM_DEBUG_MSGS
#define DBUG(x,y)	fvwm_msg(DBG,x,y)
#else
#define DBUG(x,y)	/* no messages */
#endif

/*
 ****************************************************************
 *	Arquivos & Diretórios					*
 ****************************************************************
 */
#define	FVWMRC		".fvwmrc"
#define	FVWM_ICONDIR	"/usr/xwin/pixmaps"
#define	FVWM_MODULEDIR	"/usr/xwin/bin"
#define	FVWMDIR		"/usr/xwin/conf"
