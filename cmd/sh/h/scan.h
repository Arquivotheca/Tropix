/*
 ****************************************************************
 *								*
 *			scan.h					*
 *								*
 *	Definições relativas à análise léxica			*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#ifdef	MAJOR		/* Evita conflitos com <sys/common.h> */
#undef	MAJOR
#endif	MAJOR

#define	VAR_SYMBOL	24
#define	CMD_SYMBOL	25

/*
 ****************************************************************
 *	Majors dos Tokens 					*
 ****************************************************************
 */
typedef	enum
{
	EOT,		/* Final de texto */
	EOL,		/* Final da linha */

	STRING,		/* Cadeia */
	DEFVAR,		/* Definição de variável */
	PROG,		/* Comando de programação */

	REDIREC,	/* Redirecionamento */

	/*
	 *	Atenção: a ordem dos conectores não pode ser alterada.
	 */
	BG,		/* & */
	SM,		/* ; */
	AND,		/* && */
	OR,		/* || */
	PIPE,		/* | */

	SMSM,		/* ;; */

	LP,		/* ( */
	RP,		/* ) */
	LC,		/* { */
	RC		/* } */

}	MAJOR;

/*
 ****************************************************************
 *	Estrutura de saída do Analisador Léxico			*
 ****************************************************************
 */
typedef	struct
{
	MAJOR	t_major;	/* Major do Token */

	int	t_nline;	/* Número da linha do token */

	char	*t_begin,	/* Coluna inicial do token na linha */
		*t_end;		/* Coluna final   do token na linha */

	char	*t_sval;	/* Se t_major == STRING */

	PROGID	t_progid;	/* Se t_major == PROG */

}	TOKEN;

/*
 ****************************************************************
 *	Variáveis do Analisador Léxico				*
 ****************************************************************
 */
extern	TOKEN		token;
extern	FILE		*input_file;
extern	char		*lineread;
