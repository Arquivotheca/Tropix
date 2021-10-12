/*
 ****************************************************************
 *								*
 *			<signal.h>				*
 *								*
 *	Signal handling						*
 *								*
 *	Versão	2.3.0, de 02.10.89				*
 *		4.4.0, de 28.11.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definição de tipos					*
 ****************************************************************
 */
typedef int	sig_atomic_t;	/* Tipo indivisível */

/*
 ****************************************************************
 *	Constantes 						*
 ****************************************************************
 */
#define SIG_DFL	(void (*) (int, ...))0	/* Ação normal (cancela o processo) */
#define SIG_IGN	(void (*) (int, ...))1	/* Ignora o sinal */
#define SIG_ERR	(void (*) (int, ...))-1	/* Erro */

/*
 ****************************************************************
 *	Sinais							*
 ****************************************************************
 */
typedef enum
{
	SIGNULL,	/* Sem sinal */
	SIGHUP,		/* Hangup */
	SIGINT,		/* Interrupt (^C) */
	SIGQUIT,	/* Quit (FS) */
	SIGILL,		/* Instrução Invalida */
	SIGTRAP,	/* Trace ou breakpoint */
	SIGIOT,		/* I/O Trap */
	SIGEMT,		/* Emulation Trap */
	SIGFPE,		/* Exceção de ponto flutuante */
	SIGKILL,	/* Kill, morte inevitavel */
	SIGBUS,		/* Bus error */
	SIGSEGV,	/* Violação de segmentação (da gerencia de memoria) */
	SIGSYS,		/* Erro nos argumentos de um System Call */
	SIGPIPE,	/* Escrita em Pipe sem leitor */
	SIGALRM,	/* Alarm de Relogio */
	SIGTERM,	/* Sinal normal do Comando "Kill" */
	SIGADR,		/* Erro de Enderecamento */
	SIGDVZ,		/* Divisao por zero */
	SIGCHK,		/* Instrução "check" */
	SIGTRPV,	/* Instrução "trapv" */
	SIGVIOL,	/* Violação de Privilegio */
	SIGCHLD, 	/* Termino de um Filho */
	SIGABRT,	/* Syscall ABORT */
	SIGUSR1,	/* Sinal Reservado para o usuario (1) */
	SIGUSR2,	/* Sinal Reservado para o usuario (2) */
	SIGWINCH,	/* Mudança de tamanho da janela */

	SIG_LAST

}	SIG_ENUM;

#define	NSIG	(SIG_LAST-1)	/* NSIG < 32 */

/*
 ****************************************************************
 *	Protótipos das funções					*
 ****************************************************************
 */
extern void		(*signal (int, void (*) (int, ...))) (int, ...);
extern int		raise (int);
extern const char	*sigtostr (int);
