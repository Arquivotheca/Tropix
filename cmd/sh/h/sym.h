/*
 ****************************************************************
 *								*
 *			sym.h					*
 *								*
 *	Estrutura da Tabela de Símbolos				*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *		3.2.3, de 17.01.00				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Informações sobre símbolos				*
 ****************************************************************
 */
#define	NOSYMP		(SYM *)0

/*
 ****	Tipos de símbolos  **************************************
 */
typedef enum
{
	S_UNDEF,		/* Tipo indefinido */
	S_VAR,			/* Variável do ambiente */
	S_FUNC,			/* Comando intrínseco */
	S_PROG			/* Comando de programação */

}	STYPE;

/*
 ******	Valores para "s_flags" **********************************
 */
#define	V_ALL		0
#define	V_EXPORT	1
#define	V_READONLY	2
#define	V_PERMANENT	4

/*
 ******	Valores para "s_funcid" *********************************
 */
typedef	enum
{
	F_NOFUNC,

	F_CD,			/* "cd" */
	F_COLON,		/* ":" */
	F_DOT,			/* "." */

	F_EDIT_HIST,		/* "eh" (funções do histórico deve ficar juntas */
	F_EXEC_HIST,		/* "xh" */
	F_GRAPHIC_HIST,		/* "gh" */
	F_SHOW_HIST,		/* "h" */

	F_EXEC,			/* "exec" */
	F_EXIT,			/* "exit" */
	F_EXPORT,		/* "export" */
	F_EVAL,			/* "eval" */
	F_FALSE,		/* "false" */
	F_LOGIN,		/* "login" */
	F_LOGOUT,		/* "logout" */
	F_READ,			/* "read" */
	F_READONLY,		/* "readonly" */
	F_REHASH,		/* "rehash" */
	F_RETURN,		/* "return" */
	F_SET,			/* "set" */
	F_SHID,			/* "shid" */
	F_SHIFT,		/* "shift" */
	F_SPY,			/* "spy" */
	F_TIMES,		/* "times" */
	F_TRAP,			/* "trap" */
	F_TRUE,			/* "true" */
	F_UMASK,		/* "umask" */
	F_UNSET,		/* "unset" */
	F_WAIT			/* "wait" */

}	FUNCID;

/*
 ******	Valores para "s_progid" *********************************
 */
typedef	enum
{
	P_NOPROG,

	P_BREAK,		/* "break" */
	P_CASE,			/* "case" */
	P_CONTINUE,		/* "continue" */
	P_DO,			/* "do" */
	P_DONE,			/* "done" */
	P_ELIF,			/* "elif" */
	P_ELSE,			/* "else" */
	P_ESAC,			/* "esac" */
	P_FI,			/* "fi" */
	P_FOR,			/* "for" */
	P_IF,			/* "fi" */
	P_IN,			/* "in" */
	P_THEN,			/* "then" */
	P_TIME,			/* "time" */
	P_UNTIL,		/* "until" */
	P_WHILE,		/* "while" */
	P_ENDOPT		/* ";;" */

}	PROGID;

/*
 ******	Estrutura de informações sobre símbolos *****************
 */
typedef	struct sym	SYM;

struct	sym
{
	STYPE		s_type;		/* Tipo */
	char		s_name[32];	/* Identificador */

	int		s_union;	/* Pode ser: FUNCID, PROGID ou int */

	char		*s_value;	/* Valor da variável */

	SYM		*s_next,	/* Próxima variável */
			*s_prev;	/* Variável anterior */

	SYM		*s_hnext;	/* Próximo na cadeia de colisões */
};

/*
 ******	Algumas abreviaturas ************************************
 */
#define	s_flags		s_union
#define	s_funcid	s_union
#define	s_progid	s_union

/*
 ****************************************************************
 *	A tabela hash						*
 ****************************************************************
 */
extern	SYM		*hashtb[];
extern	const int	hashsz;

/*
 ****************************************************************
 *	Variáveis Importantes					*
 ****************************************************************
 */
extern	SYM	*home_sp,		/* Ponteiro para variável HOME */
		*ps1_sp,		/* Ponteiro para variável PS1 */
		*ps2_sp,		/* Ponteiro para variável PS2 */
		*current_prompt,	/* Prompt corrente */
		*hist_sp,		/* Ponteiro para variável HIST */
		*path_sp;		/* Ponteiro para variável PATH */

/*
 ****************************************************************
 *	Funções relacionadas à Manipulação da Tabela		*
 ****************************************************************
 */
extern	void	init_var_list (void);
extern	SYM	*define_variable_from_string (char *);
extern	SYM	*define_variable (const char *, const char *, int);
extern	int	set_variable_flag (const char *, int);
extern	SYM	*search_sym (const char *);
extern	SYM	*put_sym (const char *);
extern	int	delete_variable (const char *);
extern	void	show_variables (FILE *, int);
extern	int	show_environment (FILE *);
