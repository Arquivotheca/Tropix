/*
 ****************************************************************
 *								*
 *			error.h					*
 *								*
 *	Definições Acerca de Mensagens de Erro			*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.6.0, de 27.08.04				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Tipo de Erro ********************************************
 */
typedef	enum
{
	ADVR,			/* Advertência */
	ERRO,			/* Erro */
	COMP			/* Erro do Preprocessador */

}	ERRTYPE;

/*
 ******	Tipo de Marca *******************************************
 */
typedef	enum
{
	NOMARK,			/* Sem indicação do local do erro */
	TOKMARK,		/* Marca no Token Atual */
	MYMARK			/* São fornecidas as posições da marca */

}	MARKTYPE;

extern	int	nerrors;	/* Número de Erros Encontrados */

/*
 ****** Variáveis globais ***************************************
 */
extern const char	**color_vector; /* Para as cores dos modos */

/*
 ******	Protótipos de funções ***********************************
 */
extern void		err_msg (ERRTYPE, MARKTYPE, const char *, ...);
extern void		putmark (MARKTYPE, const char *, const char *);
