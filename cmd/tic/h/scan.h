/*
 ****************************************************************
 *								*
 *			scan.h					*
 *								*
 *	Tipos Usados pela Analise Lexica			*
 *								*
 *	Versão 2.0.0, de 21.04.87				*
 *	       3.0.0, de 12.07.97				*
 *								*
 *	Modulo: tic						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Estrutura de saida do Analisador Lexico			*
 ****************************************************************
 */
typedef enum { false, true }	bool;

typedef	struct
{
	int	t_lineno;	/* Numero da linha do token */
	char	*t_begin;	/* Coluna inicial do token na linha */
	char	*t_end;		/* Coluna final do token na linha */

}	TOKEN;

#define		LINESZ		512
#define		STRSZ		512

/*
 ******	Protótipos de funções ***********************************
 */
extern bool		readline (void);
extern int		get_cap (char *);
extern int		get_number (void);
extern bool		get_string (char []);
extern bool		get_names (char[]);
extern void		skip (bool);
