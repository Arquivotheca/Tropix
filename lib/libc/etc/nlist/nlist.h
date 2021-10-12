/*
 ****************************************************************
 *								*
 *			<nlist.h>				*
 *								*
 *	Estrutura usada pela rotina "nlist.c"			*
 *								*
 *	Versão	1.0.0, de 20.11.86				*
 *		4.0.0, de 22.08.01				*
 *								*
 *	Módulo: nlist.h						*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definição da estrutura **********************************
 */
typedef struct nlist
{
	const char	*n_name;	/* Nome do Símbolo */
	long		n_value;	/* Seu valor */
	int		n_type;		/* Seu tipo */

}	NLIST;

/*
 ******	Protótipos de funções ***********************************
 */
extern int	nlist (const char *, NLIST *);
