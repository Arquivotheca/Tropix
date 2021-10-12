/*
 ****************************************************************
 *								*
 *			sigtostr.c				*
 *								*
 *	Obtém a descrição simbólica de um sinal			*
 *								*
 *	Versão	1.0.0, de 17.10.86				*
 *		4.4.0, de 18.11.02				*
 *								*
 *	Módulo:	sigtostr 					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <signal.h>

/*
 ****************************************************************
 *	Tabela dos sinais					*
 ****************************************************************
 */
static const char * const sigstr[] =
{
    /* 0 */	"SIGNULL: Sinal NULO",
    /* 1 */	"SIGHUP:  Hangup",
    /* 2 */	"SIGINT:  Interrupção",
    /* 3 */	"SIGQUIT: Quit",
    /* 4 */	"SIGILL:  Instrução inválida",
    /* 5 */	"SIGTRAP: Trace ou breakpoint",
    /* 6 */	"SIGIOT:  Exceção IOT",
    /* 7 */	"SIGEMT:  Exceção EMT",
    /* 8 */	"SIGFPE:  Exceção de ponto flutuante",
    /* 9 */	"SIGKILL: KILL, término inevitável",
    /* 10 */	"SIGBUS:  Bus error",
    /* 11 */	"SIGSEGV: Violação de segmentação",
    /* 12 */	"SIGSYS:  Erro nos argumentos de uma chamada ao sistema",
    /* 13 */	"SIGPIPE: Escrita em \"pipe\" sem leitor",
    /* 14 */	"SIGALRM: Alarme de relógio",
    /* 15 */	"SIGTERM: Sinal normal de término",
    /* 16 */	"SIGADR:  Erro de endereçamento",
    /* 17 */	"SIGDVZ:  Divisão por ZERO",
    /* 18 */	"SIGCHK:  Instrução \"check\"",
    /* 19 */	"SIGTRPV: Instrução \"trapv\"",
    /* 20 */	"SIGVIOL: Violação de privilégio",
    /* 21 */	"SIGCHLD: Término de um filho",
    /* 22 */	"SIGABRT: Chamado ao sistema \"abort\"",
    /* 23 */	"SIGUSR1: Sinal reservado para o usuário",
    /* 24 */	"SIGUSR2: Sinal reservado para o usuário",
    /* 25 */	"SIGWINCH: Mudança do tamanho da janela"
};

/*
 ****************************************************************
 *	Ponto de entrada da função				*
 ****************************************************************
 */
const char *
sigtostr (int signum)
{
	if (signum >= 0 && signum <= NSIG)
		return (sigstr[signum]);

	return ("Sinal desconhecido");

}	/* end sigtostr */
