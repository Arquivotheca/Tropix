/*
 ****************************************************************
 *								*
 *			<ftw.h>					*
 *								*
 *	Caminha em uma árvore do sistema de arquivos		*
 *								*
 *	Versão	1.0.0, de 16.07.86				*
 *		4.2.0, de 08.04.02				*
 *								*
 *	Módulo: ftw.h						*
 *		/usr/include 					*
 *		Categoria B					*
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
typedef enum
{
	FTW_F = 5,	/* Arquivo */ 
	FTW_D,		/* Diretório */
	FTW_NS,		/* Não foi possível obter o estado */
	FTW_DNR,	/* Não foi possível abrir o diretório */
	FTW_SL		/* Elo simbólico */

}	FTW_ENUM;

/*
 ****************************************************************
 *	Protótipos das funções					*
 ****************************************************************
 */
extern int	ftw (const char *, int (*) (const char *, const STAT *, int));
