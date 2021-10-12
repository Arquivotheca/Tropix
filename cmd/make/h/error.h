/*
 ****************************************************************
 *								*
 *			error.h					*
 *								*
 *	Definições relativas às mensagens de erro		*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Tipos de erro						*
 ****************************************************************
 */
typedef	enum
{
	WARNING,	/* Advertência */
	ERROR,		/* Erro */
	INTERNAL_ERROR	/* Erro interno do programa */

}	ERRTYPE;

/*
 ****************************************************************
 *	Tipo de indicação do erro				*
 ****************************************************************
 */
typedef	enum
{
	NOMARK,		/* Sem indicação do local do erro */
	GIVENMARK,	/* Indicador com as posições dadas em "set_mark" */
	TOKMARK,	/* Indicador com a extensão do token corrente */
	SHORTMARK	/* Indicador de um caractere ('^') */

}	MARKTYPE;

/*
 ****************************************************************
 *	Estrutura de uma faixa					*
 ****************************************************************
 */
struct	range
{
	char	*r_begin,	/* Posição inicial da faixa */
		*r_end,		/* Posição final   da faixa */
		*r_line;	/* Posição do início da linha onde
				   começa a faixa */
	int	r_nline;	/* Número da linha onde começa a faixa */
};
