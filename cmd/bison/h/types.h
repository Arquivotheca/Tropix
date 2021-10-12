
/*
 ****************************************************************
 *								*
 *			types.h					*
 *								*
 *  Data type for repres. bison's grammar input as it is parsed	*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *  Data type for repres. bison's grammar input as it is parsed	*
 ****************************************************************
 */
typedef
  struct shorts
    {
      struct shorts *next;
      short value;
    }
  shorts;
