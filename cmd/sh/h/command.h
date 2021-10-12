/*
 ****************************************************************
 *								*
 *			command.h				*
 *								*
 *	Estrutura para representação dos comandos		*
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

/*
 ****************************************************************
 *	Estrutura para a representação do redirecionamento	*
 ****************************************************************
 */
#define	NORDRC		(RDRC *)NULL

typedef	struct redirec	RDRC;

/*
 ******	Tipos de redirecionamentos ******************************
 */
typedef	enum
{
	R_ERROR,	/* redirecionamento inválido */
	R_DEL,		/* [digito]{<>}&- */
	R_FROM,		/* [digito]<&digito */
	R_TO,		/* [digito]>&digito */
	R_IN,		/* [digito]< nome */
	R_OUT,		/* [digito]> nome} */
	R_APP,		/* [digito]>> nome} */
	R_SIN		/* [digito]<< nome} */

}	RTYPE;

struct	redirec
{
	RTYPE		r_type;		/* Tipo de redirecionamento */
	int		r_fd;		/* FD a ser criado ou fechado (&-) */

	union
	{
		int	r_dupfd;	/* FD a ser dupped */
		char	*r_name;	/* Arquivo a ser aberto ou
					   marca do fim do texto (<<) */
	}	r;

	RDRC		*r_next;	/* Próximo redirecionamento */
};

extern	RDRC		rdrc;

/*
 ****************************************************************
 *	Estrutura para a representação dos comandos		*
 ****************************************************************
 */
#define	NOCMD		(CMD *)0
#define	ERRCMD		(CMD *)(-1)
#define	ENDCMD		(CMD *)(-2)

typedef	struct command	CMD;

/*
 ******	Tipos de comandos ***************************************
 */
typedef	enum
{	/*
	 *	Folhas da árvore.
	 */
	T_BREAK,	/* break */
	T_CONTINUE,	/* continue */

	T_ASSIGN,	/* atribuições de variáveis */
	T_SIMPLE,	/* c_argv < c_input > c_output */

	/*
	 *	Nós interiores da árvore.
	 */
	T_LIST,		/* c_leftcmd ; c_rightcmd */
	T_AND,		/* c_leftcmd && c_rightcmd */
	T_OR,		/* c_leftcmd || c_rightcmd */
	T_PIPE,		/* c_leftcmd | c_rightcmd */

	T_CASE,		/* case ... */
	T_FOR,		/* for ... */
	T_IF,		/* if ... */
	T_UNTIL,	/* until ... */
	T_WHILE		/* while ... */

}	CMDTYPE;

/*
 ******	Modificadores dos comandos ******************************
 */
#define		M_NOFLAGS	0
#define		M_PIPEIN	(1 << 0)
#define		M_PIPEOUT	(1 << 1)
#define		M_BACK		(1 << 2)
#define		M_SUBSHELL	(1 << 4)
#define		M_TIME		(1 << 5)

struct	command
{
	CMDTYPE		c_type;		/* Tipo do comando */

	int		c_flags;	/* Modificadores do comando */

	int		c_fdvector;	/* Uso dos fd's */

	RDRC		*c_rdrc;	/* Redirecionamentos */

	int		c_argc;		/* Número de argumentos */

	char		**c_argv,	/* Argumentos para o comando */
			**c_assign;	/* Atribuições de parâmetros */

	FUNCID		c_fid;		/* Função interna */

	CMD		*c_leftcmd,	/* Comando à esquerda */
			*c_rightcmd,	/* Comando à direita */
			*c_parent;	/* Nó pai */
};
