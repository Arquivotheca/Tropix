/*
 ****************************************************************
 *								*
 *			extern.h				*
 *								*
 *	Variáveis Globais					*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *		4.5.0, de 10.10.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Em "src/exec.c" *****************************************
 */
extern	int		sonsignaled;

extern	int		last_bg_pid,	/* PID do último cmd em background */
			last_cmd_code;	/* Valor de retorno do último comando */

/*
 ******	Em "src/hist.c" *****************************************
 */
extern	int		hist_max;	/* Tamanho máximo do histórico */
extern	char		HISTMAX[];	/* Cadeia contendo o tam. máximo */
extern	int		last_cmd;	/* Número do último comando */

/*
 ******	Em "src/parse.c" ****************************************
 */
extern	int		in_command;	/* Analisando um comando */
extern	int		nesting;	/* Nível de aninhamento */

/*
 ******	Em "src/scan.c" *****************************************
 */
extern	TOKEN		token;		/* Token corrente */

extern	char		*linebuf,
			*line;

extern	int		nline;
extern	int		lineprinted;
extern	int		line_read;	/* A linha já foi lida */

/*
 ******	Em "src/sh.c" *******************************************
 */
extern	const char	pgversion[];	/* Versão do Programa */

extern	int		exit_code;	/* Código de retorno */

extern	int		dflag,		/* Sai com Control-D */
			quick_exit,	/* Termina se der erro em comando */
			rewriting,	/* Verifica a reescrita */
			vflag;		/* Verboso */

extern	int		script,		/* Executando arquivo de comandos */
			interact;	/* Interativa */

extern	int		my_pid,		/* PID da Shell */
			user_id;	/* UID */

extern	int		max_fd;		/* Número máximo de FD's */

extern	int		npospar;	/* Número de Parâmetros Posicionais */
extern	const char	*pospar[];	/* Parâmetros Posicionais */

extern	int		ignore_signals;		/* Ignora INT, QUIT e TERM */
extern	const char	*trap[];		/* Comandos associados ao sinais */
extern	void		(*sig[]) (int, ...);	/* Estados antigos dos sinais */

extern	char		*current_wd;	/* Nome do diretório corrente */
